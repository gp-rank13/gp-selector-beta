// Rackspace and song selector extension for Gig Performer by @rank13

#include "LibMain.h"
#include "Panels.h"
#include <regex>

namespace gigperformer {
   namespace sdk {
      GigPerformerAPI *CreateGPExtension(LibraryHandle handle) {
         return new LibMain(handle);
      }
   } // namespace sdk
} // namespace gigperformer

bool isGigFileLoading = false;
bool isFirstGigFileOpened = true;

int LibMain::GetMenuCount()
{
   return menuNames.size();
}

 std::string  LibMain::GetMenuName(int index)
{
   std::string text;
   if (index >= 0 && index < (int)menuNames.size())
       text =  menuNames[index];
         
   return text;      
}

void LibMain::InvokeMenu(int index)
{
   if (index >= 0 && index < (int)menuNames.size())
         {
            switch (index)
               {
                  case 0:
                      ExtensionWindow::displayWindow(true);
                     break;
                  case 1:
                     ExtensionWindow::displayWindow(false);
                     break;
                  case 2:
                    ExtensionWindow::toggleZeroBasedNumbering();
                    break;
                  case 3:
                    ExtensionWindow::toggleImmediateSwitching();
                    break;
                  case 4:
                    ExtensionWindow::toggleLargeScrollArea();
                    break;
                  case 5:
                    ExtensionWindow::toggleThickBorders();
                    break;
                  default:
                     break;   
               }
         }
}

int LibMain::GetPanelCount() {
   return panelNames.size();
}

std::string LibMain::GetPanelName(int index) {
   std::string text; 
   size_t panelIndex = (size_t)index;
   if (panelIndex >= 0 && panelIndex < panelNames.size())
      text = panelNames[index];
         
   return text;      
}

std::string LibMain::GetPanelXML(int index) {
   std::string text;
   size_t panelIndex = (size_t)index;
   if (panelIndex >= 0 && panelIndex < panelNames.size()) {
        text = panelXML[index];
   }
   return text;
}

std::vector<std::string> LibMain::getSongNames() {
    std::vector<std::string> songNames;
    std::string songName;
    for (int i = 0; i < getSongCount(); ++i) { 
        songName = getSongName(i);
        songNames.push_back(songName);
    }
    return songNames;
}

std::vector<std::string> LibMain::getSongPartNames(int songIndex) {
    std::vector<std::string> songPartNames;
    std::string songPartName;
    for (int i = 0; i < getSongpartCount(songIndex); ++i) { 
        songPartName = getSongpartName(songIndex, i);
        songPartNames.push_back(songPartName);
    }
    return songPartNames;
}

std::vector<std::string> LibMain::getRackspaceNames() {
    std::vector<std::string> rackspaceNames;
    std::string rackspaceName;
    for (int i = 0; i < getRackspaceCount(); ++i) { 
        rackspaceName = getRackspaceName(i);
        rackspaceNames.push_back(rackspaceName);
    }
    return rackspaceNames;
}

std::vector<std::string> LibMain::getVariationNames(int rackspaceIndex) {
    std::vector<std::string> variationNames;
    std::string variationName;
    for (int i = 0; i < getVariationCount(rackspaceIndex); ++i) { 
        variationName = getVariationName(rackspaceIndex, i);
        variationNames.push_back(variationName);
    }
    return variationNames;
}

std::vector<std::vector<std::string>> LibMain::getAllVariationNames() {
    std::vector<std::vector<std::string>> allVariationNames;
    std::vector<std::string> variationNames;
    for (int i = 0; i < getRackspaceCount(); ++i) { 
        variationNames = getVariationNames(i);
        allVariationNames.push_back(variationNames);
    }
    return allVariationNames;
}

void LibMain::OnStatusChanged(GPStatusType status) {
    switch (status) {
        case GPStatus_GigStartedLoading:
            isGigFileLoading = true;
            break;
        case GPStatus_GigFinishedLoading:
            isGigFileLoading = false;
            if (isFirstGigFileOpened) {
                readPreferencesFile("");
                isFirstGigFileOpened = false;
            }
            ExtensionWindow::refreshUI();
            break;
        case GPStatus_RackspaceListModified:
            //consoleLog("Rackspace list changed");
            break;
        case GPStatus_SongListModifed:
            //consoleLog("Song list changed");
            break;
        case GPStatus_SongPartListModified:
            //consoleLog("SongPart list changed");
            break;
        case GPStatus_VariationListModified:
            //consoleLog("Variation list changed");
            break;
        default:
            break;
    }
}

void LibMain::OnOpen() {
    ExtensionWindow::initialize();
}

void LibMain::OnClose() {
    ExtensionWindow::finalize();
}

void LibMain::OnRackspaceActivated() {
    if (isGigFileLoading) return;
    if (!inSetlistMode()) {
        int index = getCurrentRackspaceIndex();
        if (index >= 0) {
            ExtensionWindow::updateButtonNames(getRackspaceNames());
            if (!ExtensionWindow::isButtonSelected(index)) { // If selected in GP directly, ensure buttons are in sync
                ExtensionWindow::selectButton(index);
                ExtensionWindow::updateSubButtonNames(getVariationNames(index));
                ExtensionWindow::selectSubButton(getCurrentVariationIndex());
            } else {
                ExtensionWindow::updateSubButtonNames(getVariationNames(index));
            }
        }
    }
}

void LibMain::OnVariationChanged(int oldIndex, int newIndex) {
    if (isGigFileLoading) return;
    if (newIndex >= 0 && oldIndex != newIndex && !inSetlistMode()) {
        int rackspaceIndex = getCurrentRackspaceIndex();
        if (!ExtensionWindow::isSubButtonSelected(newIndex)) {
            ExtensionWindow::selectSubButton(newIndex);
        ExtensionWindow::updateSubButtonNames(getVariationNames(rackspaceIndex));
        }
    }
}

void LibMain::OnSongChanged(int, int newIndex) {
    if (isGigFileLoading) return;
    if (newIndex >= 0 && inSetlistMode()) {
        ExtensionWindow::updateButtonNames(getSongNames());
        ExtensionWindow::chordProReadFile(newIndex);
        setWidgetValue(WIDGET_CP_SCROLL, 0.0);
        if (!ExtensionWindow::isButtonSelected(newIndex)) { // If selected in GP directly, ensure buttons are in sync
            ExtensionWindow::selectButton(newIndex);
            ExtensionWindow::updateSubButtonNames(getSongPartNames(newIndex));
            ExtensionWindow::selectSubButton(getCurrentSongpartIndex());
        } else {
            ExtensionWindow::updateSubButtonNames(getSongPartNames(newIndex));
        }
    }
}

void LibMain::OnSongPartChanged(int oldIndex, int newIndex) {
    if (isGigFileLoading) return;
    if (newIndex >= 0 && oldIndex != newIndex && inSetlistMode()) {
        int songIndex = getCurrentSongIndex();
        if (!ExtensionWindow::isSubButtonSelected(newIndex)) {
            ExtensionWindow::updateSubButtonNames(getSongPartNames(songIndex));
            ExtensionWindow::selectSubButton(newIndex);
            ExtensionWindow::chordProScrollToSongPart(getSongpartName(getCurrentSongIndex(),newIndex));
        }
    }
}

void LibMain::OnSetlistChanged(const std::string&) {
    if (isGigFileLoading) return;
    if (inSetlistMode()) {
        ExtensionWindow::updateButtonNames(getSongNames());
        ExtensionWindow::selectButton(getCurrentSongIndex());
    }
}

void LibMain::OnModeChanged(int mode) {
    if (isGigFileLoading) return;
    readPreferencesFile("colors");
    if (mode == GP_SetlistMode) setWidgetValue(WIDGET_CP_SCROLL, 0.0);
    ExtensionWindow::refreshUI();
}

// Handle widget changes
void LibMain::OnWidgetValueChanged(const std::string& widgetName, double newValue) {
    if (widgetName == WIDGET_SELECTOR) {
        if (newValue == 1.0) {
         ExtensionWindow::displayWindow(true);
        } else {
            ExtensionWindow::displayWindow(false);
        }
    } else if (widgetName == WIDGET_SCROLL) {
            ExtensionWindow::scrollWindow(newValue);
    } else if (widgetName == WIDGET_CP_SCROLL) {
            ExtensionWindow::chordProScrollWindow(newValue);
    } else if(widgetName == WIDGET_CP_UP && newValue == 1.0) {
        ExtensionWindow::chordProUp();
    } else if(widgetName == WIDGET_CP_DOWN && newValue == 1.0) {
        ExtensionWindow::chordProDown();
    } else if(widgetName == WIDGET_HUD) {
        ExtensionWindow::displayHUD(newValue == 1.0 ? true : false);
    }
}

void LibMain::readPreferencesFile(std::string onlySection = "") {
    std::string prefsFileText;
    gigperformer::sdk::GPUtils::loadTextFile(getPathToMe() + PATH_SEPARATOR() + PREF_FILENAME, prefsFileText);
    StringArray lines = StringArray::fromLines(prefsFileText);
    StringArray keyValue;
    StringPairArray defaults;
    StringPairArray colors;
    StringPairArray chordpro;
    String line;
    String prefSection;
    for (int i = 0; i < lines.size(); ++i) { 
        line = lines[i].toStdString();
        if (line.contains("[")) { // Preference Heading/Section
            prefSection = line.removeCharacters("[]");
        } else if (line.trim() != "") { // Process Preferences, assuming key/value pairs
            line = line.removeCharacters(" ");
            keyValue = StringArray::fromTokens(line,"=","");
            if (prefSection == "Defaults") {
                defaults.set(keyValue[0], keyValue[1]);
            } else if (prefSection == "SongPartVariationColors" || prefSection == "Colors") {
                colors.set(keyValue[0], keyValue[1]);
            } else if (prefSection.contains("ChordPro")) {
                chordpro.set(prefSection + keyValue[0], keyValue[1]);
            }
        }
    }
    if (onlySection == "defaults" || onlySection == "") {
        ExtensionWindow::processPreferencesDefaults(defaults);
    }
    if (onlySection == "colors" || onlySection == "") {
        ExtensionWindow::processPreferencesColors(colors);
    }
    if (onlySection == "chordpro" || onlySection == "") {
        ExtensionWindow::processPreferencesChordPro(chordpro);
    }
}

std::string LibMain::GetProductDescription()  // This MUST be defined in your class
    {
        return XMLProductDescription;
    }