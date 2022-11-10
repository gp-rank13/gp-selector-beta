// Rackspace and song selector extension for Gig Performer by @rank13

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// Script logger prefix
const std::string LOG_PREFIX = "[GPSelector] ";

// Define the file separator (Windows vs Mac) to be used when reading the preferences file
inline char PATH_SEPARATOR() {
    #if JUCE_WINDOWS
        return '\\';
    #else
        return '/';
    #endif
}

// Window title bar and header labels
const std::string RACKSPACE_TITLE = "Rackspaces";
const std::string RACKSPACE_WINDOW_TITLE = "Rackspace Selector";
const std::string SONG_TITLE = "Songs";
const std::string SONG_WINDOW_TITLE = "Song Selector";

// Widget names used in the extension
const std::string WIDGET_SELECTOR = "GP_SELECTOR";
const std::string WIDGET_SCROLL = "GP_SELECTOR_SCROLL";
const std::string WIDGET_CP_SCROLL = "GP_SELECTOR_CP_SCROLL";
const std::string WIDGET_CP_UP = "GP_SELECTOR_CP_UP";
const std::string WIDGET_CP_DOWN = "GP_SELECTOR_CP_DOWN";

// List of menu items
const std::vector<std::string> menuNames = { "Show Selector", "Hide Selector", "Toggle Zero-Based Numbers", "Toggle Immediate Switching", "Toggle Large Scroll Area", "Toggle Thick Borders"};

// Preferences file
const std::string PREF_FILENAME = "GPSelectorPrefs.txt";

// Default number of rackspaces/song and variations/song part buttons that are created by the extension
const int DEFAULT_RACKSPACES_SONGS = 50;
const int DEFAULT_VARIATIONS_SONGPARTS = 10;

// GUI defaults
const std::string DEFAULT_WINDOW_POSITION = "100,100,300,600";
const juce::String DEFAULT_BUTTON_COLOR = "FF3F3F3F";
const juce::String DEFAULT_SUBBUTTON_COLOR = "FF353535";
const juce::String DEFAULT_BORDER_COLOR = "FFE5E5E5";
const juce::String BACKGROUND_COLOR = "FF2A2A2A";
const juce::String WINDOW_ICON = "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";

const int MIN_WINDOW_WIDTH = 180;
const int SCROLL_AREA_WIDTH = 50;
const int HEADER_HEIGHT = 50;
const int PANE_SEPARATOR_WIDTH = 15;

// ChordPro defaults
const float CP_DEFAULT_FONT_SIZE = 0.7f;
const float CP_MIN_FONT_SIZE = 0.4f;
const float CP_MAX_FONT_SIZE = 1.6f;
const float CP_FONT_STEP_SIZE = 0.1f;
const int CP_DEFAULT_IMAGES = 10;

const juce::String CP_LIGHT_LYRIC_COLOR = "FF000000";
const juce::String CP_LIGHT_CHORD_COLOR = "FFFC0000";
const juce::String CP_LIGHT_BACKGROUND_COLOR = "FFFFFFFF";
const juce::String CP_LIGHT_IMAGES_BACKGROUND_COLOR = "FFF0F0F0";
const juce::String CP_DARK_LYRIC_COLOR = "FFFFFFFF";
const juce::String CP_DARK_CHORD_COLOR = "FFFDFB00";
const juce::String CP_DARK_BACKGROUND_COLOR = "FF000000";
const juce::String CP_DARK_IMAGES_BACKGROUND_COLOR = BACKGROUND_COLOR;

const juce::String CP_DARK_MODE_FILE_SUFFIX = "-DarkMode";
const juce::String CP_NO_DARK_MODE_IMAGES = "No dark mode image";
const juce::String CP_DARK_MODE_IMAGE_PROCESSING = "Creating image...";

const std::vector<std::string> NOTES_SHARP = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
const std::vector<std::string> NOTES_FLAT = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};

#endif