// Rackspace and song selector extension for Gig Performer by @rank13

#pragma once

#include <gigperformer/sdk/GigPerformerAPI.h>
#include <gigperformer/sdk/GPUtils.h>
//#include <juce_gui_basics/juce_gui_basics.h>
#include "ExtensionWindow.h"
#include "Constants.h"

// Define an XML string describing your product
const std::string XMLProductDescription =
    "<Library>"
    "<Product Name=\"GP Selector\" Version=\"1.6b2\" BuildDate=\"02/09/2022\"></Product> "
    "<Description>Rackspace and Song Selector</Description>"
    "<ImagePath></ImagePath>"
    "</Library>";

// Define your class here - it MUST be called LibMain and it must inherit from GigPerformerAPI
class LibMain : public gigperformer::sdk::GigPerformerAPI
{
protected:
    // These are for creating menu items in Gig Performer that can be used to trigger external functions provided by the extension developer
    int GetMenuCount() override;
    std::string GetMenuName(int index) override;
    void InvokeMenu(int itemIndex) override;

    // These are for creating the custom panel template
    int GetPanelCount() override;
    std::string GetPanelName(int index) override;
    std::string GetPanelXML(int index) override;

public:
    // These must be here but no need to do anything unless you want extra behavior
    LibMain();
    LibMain(LibraryHandle handle) : GigPerformerAPI(handle)  {}
    virtual ~LibMain() {}
    void OnOpen() override;
    void OnClose() override;
    void OnRackspaceActivated() override;
    void OnVariationChanged(int oldIndex, int newIndex) override;
    void OnModeChanged(int mode) override;
    void OnSongChanged(int oldIndex, int newIndex) override;
    void OnSongPartChanged(int oldIndex, int newIndex) override;
    void OnSetlistChanged(const std::string &newSetlistName) override;
    void OnWidgetValueChanged(const std::string &widgetName, double newValue) override;    
    void OnStatusChanged(GPStatusType status) override; 
    void Initialization() override
    {
        registerCallback("OnOpen");
        registerCallback("OnClose");
        registerCallback("OnStatusChanged");
        registerCallback("OnRackspaceActivated");
        registerCallback("OnVariationChanged");
        registerCallback("OnSongChanged");
        registerCallback("OnSongPartChanged");
        registerCallback("OnSetlistChanged");
        registerCallback("OnModeChanged");
        registerCallback("OnWidgetValueChanged");

        listenForWidget(WIDGET_SELECTOR, true);
        listenForWidget(WIDGET_SCROLL, true);
        listenForWidget(WIDGET_CP_SCROLL, true);
        listenForWidget(WIDGET_CP_UP, true);
        listenForWidget(WIDGET_CP_DOWN, true);
        listenForWidget(WIDGET_HUD, true);
    }
    
    std::string GetProductDescription() override; // This MUST be defined in your class
    StringArray getRackspaceNames();
    StringArray getVariationNames(int rackspaceIndex);
    StringArray getSongNames();
    StringArray getSongPartNames(int songIndex);
private:
    StringArray getAllVariationNames();
    void readPreferencesFile(String onlySection);
};