/* Copyright (C) 2003-2015 LiveCode Ltd.
 
 This file is part of LiveCode.
 
 LiveCode is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License v3 as published by the Free
 Software Foundation.
 
 LiveCode is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.
 
 You should have received a copy of the GNU General Public License
 along with LiveCode.  If not see <http://www.gnu.org/licenses/>.  */

#ifndef __MC_PLATFORM_INTERNAL__
#define __MC_PLATFORM_INTERNAL__

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

class MCPlatformBase
{
public:
    MCPlatformBase(void);
    virtual ~MCPlatformBase(void);
    
    virtual void Retain(void);
    virtual void Release(void);
private:
    uint32_t m_references;
};

// MM-2014-07-31: [[ ThreadedRendering ]] Updated to match the new stack surface API.
//  You can now lock/unlock multiple areas of the surface, but need to store the context and raster for those areas locally.
class MCPlatformSurface
{
public:
	MCPlatformSurface(void);
	virtual ~MCPlatformSurface(void);
	
	virtual bool LockGraphics(MCGIntegerRectangle area, MCGContextRef& r_context, MCGRaster &r_raster) = 0;
	virtual void UnlockGraphics(MCGIntegerRectangle area, MCGContextRef context, MCGRaster &raster) = 0;
	
	virtual bool LockPixels(MCGIntegerRectangle area, MCGRaster& r_raster, MCGIntegerRectangle &r_locked_area) = 0;
	virtual void UnlockPixels(MCGIntegerRectangle area, MCGRaster& raster) = 0;
	
	virtual bool LockSystemContext(void*& r_context) = 0;
	virtual void UnlockSystemContext(void) = 0;
	
	virtual bool Composite(MCGRectangle dst_rect, MCGImageRef src_image, MCGRectangle src_rect, MCGFloat opacity, MCGBlendMode blend) = 0;

	virtual MCGFloat GetBackingScaleFactor(void) = 0;
};

////////////////////////////////////////////////////////////////////////////////

class MCPlatformWindowMask: public MCPlatformBase
{
public:
    MCPlatformWindowMask(void);
    virtual ~MCPlatformWindowMask(void);
    
    virtual bool IsValid(void) const = 0;

    virtual bool CreateWithAlphaAndRelease(int32_t p_width, int32_t p_height, int32_t p_stride, void *p_bits) = 0;
};

////////////////////////////////////////////////////////////////////////////////

typedef void (*MCPlatformWindowAttachmentCallback)(void *object, bool realized);

struct MCPlatformWindowAttachment
{
	void *object;
	MCPlatformWindowAttachmentCallback callback;
};

class MCPlatformWindow: public MCPlatformBase
{
public:
	MCPlatformWindow(void);
	virtual ~MCPlatformWindow(void);
	
	// Returns true if the window is being shown.
	bool IsVisible(void);
	
	// Force an immediate update of the window's dirty region. Assuming there
	// is updates to be made, this will cause an immediate redraw callback.
	void Update(void);
	
	// Add the given region to the window's dirty region.
	void Invalidate(MCGRegionRef region);
	
	// Make the window visible as the given class.
	void Show(void);
	void ShowAsSheet(MCPlatformWindowRef parent);
	
	// Make the window invisible.
	void Hide(void);
	
	// Set input focus to the window.
	void Focus(void);
	
	// Bring the window to front.
	void Raise(void);
	
	// Minimize / miniturize the window.
	void Iconify(void);
	
	// Deminimize / deminiturize the window.
	void Uniconify(void);
	
	// Manage text input sessions
	void ConfigureTextInput(bool enabled);
	void ResetTextInput(void);
	bool IsTextInputActive(void);
	
	// Set the given window property.
	void SetProperty(MCPlatformWindowProperty property, MCPlatformPropertyType type, const void *value);
	
	// Get the given window property.
	void GetProperty(MCPlatformWindowProperty property, MCPlatformPropertyType type, void *r_value);
	
	// Map co-ords from window to screen and vice-versa.
	void MapPointFromScreenToWindow(MCPoint screen_point, MCPoint& r_window_point);
	void MapPointFromWindowToScreen(MCPoint window_point, MCPoint& r_screen_point);
	
	// Attach an object that needs to be notified when the window is realized / unrealized.
	void AttachObject(void *object, MCPlatformWindowAttachmentCallback callback);
	void DetachObject(void *object);
	
public:
	
	void HandleCloseRequest(void);
	
	void HandleRedraw(MCPlatformSurfaceRef surface, MCGRegionRef update_rgn);
	void HandleReshape(MCRectangle new_content);
	void HandleIconify(void);
	void HandleUniconify(void);
	void HandleFocus(void);
	void HandleUnfocus(void);
	
	void HandleKeyDown(MCPlatformKeyCode key_code, codepoint_t mapped_codepoint, codepoint_t unmapped_codepoint);
	void HandleKeyUp(MCPlatformKeyCode key_code, codepoint_t mapped_codepoint, codepoint_t unmapped_codepoint);
	
	void HandleDragEnter(class MCRawClipboard* p_dragboard, MCPlatformDragOperation& r_operation);
	void HandleDragMove(MCPoint location, MCPlatformDragOperation& r_operation);
	void HandleDragLeave(void);
	void HandleDragDrop(bool& r_accepted);
    // Called to tell attachments there is a handle.
    void RealizeAndNotify(void);
	
	//////////
	
	virtual void DoRealize(void) = 0;
	virtual void DoSynchronize(void) = 0;
	
	virtual bool DoSetProperty(MCPlatformWindowProperty property, MCPlatformPropertyType type, const void *value) = 0;
	virtual bool DoGetProperty(MCPlatformWindowProperty property, MCPlatformPropertyType type, void *r_value) = 0;
	
	virtual void DoShow(void) = 0;
	virtual void DoShowAsSheet(MCPlatformWindowRef parent) = 0;
	virtual void DoHide(void) = 0;
	virtual void DoFocus(void) = 0;
	virtual void DoRaise(void) = 0;
	virtual void DoUpdate(void) = 0;
	virtual void DoIconify(void) = 0;
	virtual void DoUniconify(void) = 0;
	virtual void DoConfigureTextInput(void) = 0;
	virtual void DoResetTextInput(void) = 0;
	
	virtual void DoMapContentRectToFrameRect(MCRectangle content, MCRectangle& r_frame) = 0;
	virtual void DoMapFrameRectToContentRect(MCRectangle frame, MCRectangle& r_content) = 0;
	
protected:
	
	// Any attachments the window has.
	MCPlatformWindowAttachment *m_attachments;
	uindex_t m_attachment_count;
	
	// Universal property values.
	struct 
	{
		bool style_changed : 1;
		bool opacity_changed : 1;
		bool mask_changed : 1;
		bool content_changed : 1;
		bool title_changed : 1;
		bool has_title_widget_changed : 1;
		bool has_close_widget_changed : 1;
		bool has_collapse_widget_changed : 1;
		bool has_zoom_widget_changed : 1;
		bool has_size_widget_changed : 1;
		bool has_shadow_changed : 1;
		bool is_opaque_changed : 1;
		bool has_modified_mark_changed : 1;
		bool use_live_resizing_changed : 1;
        
        // MW-2014-04-08: [[ Bug 12073 ]] Changed flag for mouse cursor.
        bool cursor_changed : 1;
        
        bool hides_on_suspend_changed : 1;
        
        // MERG-2014-06-02: [[ IgnoreMouseEvents ]] Changed flag for ignore mouse events.
        bool ignore_mouse_events_changed : 1;
        
        // MERG-2015-10-11: [[ DocumentFilename ]] Changed flag for docuent filename
        bool document_filename_changed : 1;
	} m_changes;
	MCPlatformWindowStyle m_style;
	MCStringRef m_title;
	MCPlatformWindowMaskRef m_mask;
	float m_opacity;
	MCRectangle m_content;
	MCCursorRef m_cursor;
    // MERG-2015-10-11: [[ DocumentFilename ]] documentFilename property
    MCStringRef m_document_filename;
	struct
	{
		bool m_has_title_widget : 1;
		bool m_has_close_widget : 1;
		bool m_has_collapse_widget : 1;
		bool m_has_zoom_widget : 1;
		bool m_has_size_widget : 1;
		bool m_has_shadow : 1;
		bool m_is_opaque : 1;
		bool m_has_modified_mark : 1;
		bool m_use_live_resizing : 1;
        bool m_hides_on_suspend : 1;
        // MERG-2014-06-02: [[ IgnoreMouseEvents ]] ignoreMouseEvents property
        bool m_ignore_mouse_events : 1;
	};
	
	// Universal state.
	MCGRegionRef m_dirty_region;
	struct
	{
		bool m_is_visible : 1;
		bool m_is_focused : 1;
		bool m_is_iconified : 1;
		bool m_use_text_input : 1;
        bool m_is_realized : 1;
	};
};

////////////////////////////////////////////////////////////////////////////////

class MCPlatformColorTransform: public MCPlatformBase
{
public:
    MCPlatformColorTransform(const MCColorSpaceInfo& p_info);
    virtual ~MCPlatformColorTransform(void);
    
    virtual bool Apply(MCImageBitmap *p_image) = 0;
};

////////////////////////////////////////////////////////////////////////////////

class MCPlatformLoadedFont: public MCPlatformBase
{
public:
    MCPlatformLoadedFont(MCStringRef p_path, bool p_globally);
    virtual ~MCPlatformLoadedFont(void);
    
protected:
    MCStringRef m_path;
    bool m_globally;
};


////////////////////////////////////////////////////////////////////////////////

class MCPlatformCursor: public MCPlatformBase
{
public:
    MCPlatformCursor(void);
    virtual ~MCPlatformCursor(void);

    virtual void CreateStandard(MCPlatformStandardCursor p_standard_cursor);
    virtual void CreateCustom(MCImageBitmap *p_image, MCPoint p_hotspot) = 0;
    virtual void Set(void) = 0;
    
protected:
    bool is_standard : 1;
    MCPlatformStandardCursor standard;
};

////////////////////////////////////////////////////////////////////////////////

class MCPlatformMenu: public MCPlatformBase
{
public:
    MCPlatformMenu(void);
    virtual ~MCPlatformMenu(void);
    
    virtual void SetTitle(MCStringRef p_title) = 0;
    virtual uindex_t CountItems(void) = 0;
    virtual void AddItem(uindex_t p_where) = 0;
    virtual void AddSeparatorItem(uindex_t p_where) = 0;
    virtual void RemoveItem(uindex_t p_where) = 0;
    virtual void RemoveAllItems(void) = 0;
    virtual void GetParent(MCPlatformMenuRef& r_parent, uindex_t& r_index) = 0;
    virtual void GetItemProperty(uindex_t p_index, MCPlatformMenuItemProperty p_property, MCPlatformPropertyType p_type, void *r_value) = 0;
    virtual void SetItemProperty(uindex_t p_index, MCPlatformMenuItemProperty p_property, MCPlatformPropertyType p_type, const void *p_value) = 0;
    virtual bool PopUp(MCPlatformWindowRef p_window, MCPoint p_location, uindex_t p_item) = 0;
    virtual void StartUsingAsMenubar(void) = 0;
    virtual void StopUsingAsMenubar(void) = 0;
};

////////////////////////////////////////////////////////////////////////////////

class MCPlatformSound: public MCPlatformBase
{
public:
	MCPlatformSound(void);
	virtual ~MCPlatformSound(void);
	
    virtual bool IsValid(void) const = 0;
    
    virtual bool CreateWithData(const void *data, size_t data_size) = 0;
    
    virtual bool IsPlaying(void) const = 0;
    
    virtual void Play(void) = 0;
    virtual void Pause(void) = 0;
    virtual void Resume(void) = 0;
    virtual void Stop(void) = 0;
    
    virtual void SetProperty(MCPlatformSoundProperty property, MCPlatformPropertyType type, void *value) = 0;
    virtual void GetProperty(MCPlatformSoundProperty property, MCPlatformPropertyType type, void *value) = 0;
};

////////////////////////////////////////////////////////////////////////////////

class MCPlatformPlayer: public MCPlatformBase
{
public:
	MCPlatformPlayer(void);
	virtual ~MCPlatformPlayer(void);
	
	virtual bool GetNativeView(void *&r_view) = 0;
	virtual bool SetNativeParentView(void *p_parent_view) = 0;
	
	virtual bool IsPlaying(void) = 0;
	// PM-2014-05-28: [[ Bug 12523 ]] Take into account the playRate property
	virtual void Start(double rate) = 0;
	virtual void Stop(void) = 0;
	virtual void Step(int amount) = 0;
	
	virtual bool LockBitmap(const MCGIntegerSize &p_size, MCImageBitmap*& r_bitmap) = 0;
	virtual void UnlockBitmap(MCImageBitmap *bitmap) = 0;
	
	virtual void SetProperty(MCPlatformPlayerProperty property, MCPlatformPropertyType type, void *value) = 0;
	virtual void GetProperty(MCPlatformPlayerProperty property, MCPlatformPropertyType type, void *value) = 0;
	
	virtual void CountTracks(uindex_t& r_count) = 0;
	virtual bool FindTrackWithId(uint32_t id, uindex_t& r_index) = 0;
	virtual void SetTrackProperty(uindex_t index, MCPlatformPlayerTrackProperty property, MCPlatformPropertyType type, void *value) = 0;
	virtual void GetTrackProperty(uindex_t index, MCPlatformPlayerTrackProperty property, MCPlatformPropertyType type, void *value) = 0;
	
protected:
	virtual void Realize(void) = 0;
	virtual void Unrealize(void) = 0;
	
};

////////////////////////////////////////////////////////////////////////////////

class MCPlatformSoundRecorder: public MCPlatformBase
{
public:
	MCPlatformSoundRecorder(void);
	virtual ~MCPlatformSoundRecorder(void);
    
	virtual bool IsRecording(void);
    
    virtual void SetProperty(MCPlatformSoundRecorderProperty property, MCPlatformPropertyType type, void *value);
    virtual void GetProperty(MCPlatformSoundRecorderProperty property, MCPlatformPropertyType type, void *value);
    
    virtual void GetConfiguration(MCPlatformSoundRecorderConfiguration &r_config);
    virtual void SetConfiguration(const MCPlatformSoundRecorderConfiguration p_config);
    
    virtual void BeginDialog(void) = 0;
    virtual MCPlatformDialogResult EndDialog(void) = 0;
    virtual bool StartRecording(MCStringRef filename) = 0;
    virtual void StopRecording(void) = 0;
    virtual void PauseRecording(void) = 0;
    virtual void ResumeRecording(void) = 0;
    virtual double GetLoudness(void) = 0;
    
    virtual bool ListInputs(MCPlatformSoundRecorderListInputsCallback callback, void *context) = 0;
    virtual bool ListCompressors(MCPlatformSoundRecorderListCompressorsCallback callback, void *context) = 0;
    
protected:

    bool m_recording;;
    MCStringRef m_filename;
    
    // The recorder's current configuration settings.
     MCPlatformSoundRecorderConfiguration m_configuration;
};

////////////////////////////////////////////////////////////////////////////////

void MCPlatformWindowDeathGrip(MCPlatformWindowRef window);

////////////////////////////////////////////////////////////////////////////////

void MCPlatformCallbackSendApplicationStartup(int argc, MCStringRef *argv, MCStringRef *envp, int& r_error_code, MCStringRef& r_error_message);
void MCPlatformCallbackSendApplicationShutdown(int& r_exit_code);
void MCPlatformCallbackSendApplicationShutdownRequest(bool& r_terminate);
void MCPlatformCallbackSendApplicationRun(bool& r_continue);
void MCPlatformCallbackSendApplicationSuspend(void);
void MCPlatformCallbackSendApplicationResume(void);

void MCPlatformCallbackSendScreenParametersChanged(void);

void MCPlatformCallbackSendWindowCloseRequest(MCPlatformWindowRef window);
void MCPlatformCallbackSendWindowClose(MCPlatformWindowRef window);
void MCPlatformCallbackSendWindowCancel(MCPlatformWindowRef window);
void MCPlatformCallbackSendWindowReshape(MCPlatformWindowRef window, MCRectangle new_content);
void MCPlatformCallbackSendWindowConstrain(MCPlatformWindowRef window, MCPoint proposed_size, MCPoint& r_wanted_size);
void MCPlatformCallbackSendWindowRedraw(MCPlatformWindowRef window, MCPlatformSurfaceRef surface, MCGRegionRef dirty_rgn);
void MCPlatformCallbackSendWindowIconify(MCPlatformWindowRef window);
void MCPlatformCallbackSendWindowUniconify(MCPlatformWindowRef window);
void MCPlatformCallbackSendWindowFocus(MCPlatformWindowRef window);
void MCPlatformCallbackSendWindowUnfocus(MCPlatformWindowRef window);

void MCPlatformCallbackSendModifiersChanged(MCPlatformModifiers modifiers);
											
void MCPlatformCallbackSendMouseDown(MCPlatformWindowRef window, uint32_t button, uint32_t count);
void MCPlatformCallbackSendMouseUp(MCPlatformWindowRef window, uint32_t button, uint32_t count);
void MCPlatformCallbackSendMouseDrag(MCPlatformWindowRef window, uint32_t button);
void MCPlatformCallbackSendMouseRelease(MCPlatformWindowRef window, uint32_t button, bool was_menu);
void MCPlatformCallbackSendMouseEnter(MCPlatformWindowRef window);
void MCPlatformCallbackSendMouseLeave(MCPlatformWindowRef window);
void MCPlatformCallbackSendMouseMove(MCPlatformWindowRef window, MCPoint location);
void MCPlatformCallbackSendMouseScroll(MCPlatformWindowRef window, int32_t dx, int32_t dy);

void MCPlatformCallbackSendDragEnter(MCPlatformWindowRef window, class MCRawClipboard* p_dragboard, MCPlatformDragOperation& r_operation);
void MCPlatformCallbackSendDragLeave(MCPlatformWindowRef window);
void MCPlatformCallbackSendDragMove(MCPlatformWindowRef window, MCPoint location, MCPlatformDragOperation& r_operation);
void MCPlatformCallbackSendDragDrop(MCPlatformWindowRef window, bool& r_accepted);

void MCPlatformCallbackSendRawKeyDown(MCPlatformWindowRef window, MCPlatformKeyCode key_code, codepoint_t mapped_codepoint, codepoint_t unmapped_codepoint);

void MCPlatformCallbackSendKeyDown(MCPlatformWindowRef window, MCPlatformKeyCode key_code, codepoint_t mapped_codepoint, codepoint_t unmapped_codepoint);
void MCPlatformCallbackSendKeyUp(MCPlatformWindowRef window, MCPlatformKeyCode key_code, codepoint_t mapped_codepoint, codepoint_t unmapped_codepoint);

void MCPlatformCallbackSendTextInputQueryTextRanges(MCPlatformWindowRef window, MCRange& r_marked_range, MCRange& r_selected_range);
void MCPlatformCallbackSendTextInputQueryTextIndex(MCPlatformWindowRef window, MCPoint location, uindex_t& r_index);
void MCPlatformCallbackSendTextInputQueryTextRect(MCPlatformWindowRef window, MCRange range, MCRectangle& first_line_rect, MCRange& r_actual_range);
void MCPlatformCallbackSendTextInputQueryText(MCPlatformWindowRef window, MCRange range, unichar_t*& r_chars, uindex_t& r_char_count, MCRange& r_actual_range);
void MCPlatformCallbackSendTextInputInsertText(MCPlatformWindowRef window, unichar_t *chars, uindex_t char_count, MCRange replace_range, MCRange selection_range, bool mark);
void MCPlatformCallbackSendTextInputAction(MCPlatformWindowRef window, MCPlatformTextInputAction action);

void MCPlatformCallbackSendMenuUpdate(MCPlatformMenuRef menu);
void MCPlatformCallbackSendMenuSelect(MCPlatformMenuRef menu, uindex_t item);

void MCPlatformCallbackSendViewFocusSwitched(MCPlatformWindowRef window, uint32_t view_id);

void MCPlatformCallbackSendPlayerFrameChanged(MCPlatformPlayerRef player);
void MCPlatformCallbackSendPlayerMarkerChanged(MCPlatformPlayerRef player, MCPlatformPlayerDuration time);
void MCPlatformCallbackSendPlayerCurrentTimeChanged(MCPlatformPlayerRef player);
void MCPlatformCallbackSendPlayerFinished(MCPlatformPlayerRef player);
void MCPlatformCallbackSendPlayerBufferUpdated(MCPlatformPlayerRef player);

void MCPlatformCallbackSendSoundFinished(MCPlatformSoundRef sound);

////////////////////////////////////////////////////////////////////////////////

#endif
