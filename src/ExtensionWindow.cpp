// Rackspace and song selector extension for Gig Performer by @rank13

#include <juce_events/juce_events.h>
#include "ExtensionWindow.h"
#include "LibMain.h"
#include "IconPaths.h"

ExtensionWindow* ExtensionWindow::extension = nullptr;
//LibMain* lib = new LibMain(nullptr);   
LibMain* lib;
Colour chordProLyricColor = Colour::fromString(CP_DARK_LYRIC_COLOR);  
Colour chordProChordColor = Colour::fromString(CP_DARK_CHORD_COLOR);
Colour viewPortBackground = Colour::fromString(BACKGROUND_COLOR);
//Colour headerRackspaceColor = Colour::fromString(HEADER_RACKSPACE_COLOR);
Colour headerSongColor = Colour::fromString(HEADER_SONG_COLOR);
float chordProFontSize = CP_DEFAULT_FONT_SIZE;
int headerHeight = HEADER_HEIGHT;
int chordProTopPadding = CP_TOP_PADDING;
bool chordProMonospaceFont = false;
bool chordProSmallChordFont = false;
bool chordProLeftLabels = false;
bool lockToSetlistMode = false;
bool displayVariationsForSong = false;
extern std::string extensionPath;

ExtensionWindow::ExtensionWindow ()
{
    LookAndFeel::setDefaultLookAndFeel(buttonsLnF);
    clockTimer.startTimer(TIMER_CLOCK);
    refreshTimer.startTimer(TIMER_UI_REFRESH);
    lib = new LibMain(nullptr);
    preferences.reset (new DynamicObject);
    preferencesChordProColors.reset (new DynamicObject);
    /*
    preferences->setProperty("ImmediateSwitching", true);
    preferences->setProperty("LargeScrollArea", false);
    preferences->setProperty("ZeroBasedNumbers", false);
    preferences->setProperty("RemoveColorKeywordFromName", false);
    preferences->setProperty("ThickBorders", false);
    */
    String imageBase64Off = TOGGLE_OFF;
    Image toggleOff;

    MemoryOutputStream mosOff;
    auto resultOff = Base64::convertFromBase64(mosOff, imageBase64Off);
    if (resultOff) {
        toggleOff = ImageFileFormat::loadFrom(mosOff.getData(), mosOff.getDataSize());
    }
    String imageBase64On = TOGGLE_ON;
    Image toggleOn;
    MemoryOutputStream mosOn;
    auto resultOn = Base64::convertFromBase64(mosOn, imageBase64On);
    if (resultOn) {
        toggleOn = ImageFileFormat::loadFrom(mosOn.getData(), mosOn.getDataSize());
    }
    DrawableImage on, off;
    on.setImage(toggleOn);
    off.setImage(toggleOff);

    for (int i = 0; i < 6; ++i) {
        auto button = new DrawableButton(std::to_string(i), DrawableButton::ImageFitted);
        prefButtons.add(button);
        prefButtons[i]->setImages(&off, 0, 0, 0, &on);
        prefButtons[i]->setClickingTogglesState(true);
        prefButtons[i]->setRepaintsOnMouseActivity(true); 
        prefButtons[i]->addListener (this);
        prefButtons[i]->setColour (DrawableButton::backgroundOnColourId, Colour(0x00000000));
        preferencesContainer.addAndMakeVisible(prefButtons[i]);
    }

    for (int i = 0; i < 4; ++i) {
        auto label = new Label(std::to_string(i), "");
        prefColorLabels.add(label);
        prefColorLabels[i]->setRepaintsOnMouseActivity(true); 
        prefColorLabels[i]->setEditable (true, true, false);
        prefColorLabels[i]->setFont (Font (16.00f, Font::plain).withTypefaceStyle ("Regular"));
        prefColorLabels[i]->setColour (Label::backgroundColourId, Colour(0xff404040));
        prefColorLabels[i]->onTextChange = [this]() { updatePreferencesColors(); };

        preferencesContainer.addAndMakeVisible(prefColorLabels[i]);
    }
    /*
    prefToggle.reset (new DrawableButton("pref", DrawableButton::ImageFitted));
    prefToggle->setButtonStyle(DrawableButton::ImageFitted);
    prefToggle->setImages(&off, 0, 0, 0, &on);
    prefToggle->setClickingTogglesState(true);
    prefToggle->setTriggeredOnMouseDown(true);
    prefToggle->setRepaintsOnMouseActivity(true); 
    prefToggle->addListener (this);
    prefToggle->setColour (DrawableButton::backgroundOnColourId, Colour(0x00000000));
    //getLookAndFeel().setColour (DrawableButton::backgroundOnColourId, Colours::transparentWhite);
    */

    header.reset (new Label ("header", SONG_TITLE));
    addAndMakeVisible (header.get());
    header->setEditable (false, false, false);
    header->setBounds (0, 0, getWidth(), headerHeight);
    header->setFont (Font (25.00f, Font::plain).withTypefaceStyle ("Regular"));
    header->setLookAndFeel(headerSongsLnF);
    
    highlight.reset (new Label ("subButtonHighlight", ""));
    highlight->setEditable (false, false, false);
    highlight->setLookAndFeel(highlightLnF);

    clock.reset (new Label ("clock", "00:00"));
    addAndMakeVisible (clock.get());
    clock->setFont (Font (25.00f, Font::plain).withTypefaceStyle ("Regular"));
    clock->setJustificationType (Justification::centred);
    clock->setEditable (false, false, false);
    clock->setLookAndFeel(headerLnF);
    clock->setVisible(false);
    clock->setText(Time::getCurrentTime().toString(false, true, false, true), dontSendNotification);

    Path p;
    p.loadPathFromData (sidePanelOpenPathData, sizeof (sidePanelOpenPathData));
    sidePanelOpenButton.reset (new ShapeButton ( "sidePanelOpenButton", Colours::white, Colours::lightgrey, Colours::white ));
    sidePanelOpenButton->setShape (p, true, true, false);
    sidePanelOpenButton->setClickingTogglesState(true);
    sidePanelOpenButton->setTooltip("Display song list");
    sidePanelOpenButton->addListener (this);
    addAndMakeVisible (sidePanelOpenButton.get());

    Path p2;
    p2.loadPathFromData (sidePanelClosePathData, sizeof (sidePanelClosePathData));
    sidePanelCloseButton.reset (new ShapeButton ( "sidePanelCloseButton", Colours::white, Colours::lightgrey, Colours::white ));
    sidePanelCloseButton->setShape (p2, true, true, false);
    sidePanelCloseButton->setClickingTogglesState(true);
    sidePanelCloseButton->setTooltip("Hide song list");
    sidePanelCloseButton->addListener (this);
    addAndMakeVisible (sidePanelCloseButton.get());
    sidePanelCloseButton->setVisible(false);

    Path p3;
    p3.loadPathFromData (pinUnpinnedPathData, sizeof (pinUnpinnedPathData));
    p3.applyTransform(juce::AffineTransform::rotation(3.14159));
    pinUnpinnedButton.reset (new ShapeButton ( "pinUnpinnedButton", Colours::white, Colours::lightgrey, Colours::white ));
    pinUnpinnedButton->setShape (p3, true, true, false);
    pinUnpinnedButton->setClickingTogglesState(true);
    pinUnpinnedButton->setTooltip("Keep window on top");
    pinUnpinnedButton->addListener (this);
    addAndMakeVisible (pinUnpinnedButton.get());

    Path p4;
    p4.loadPathFromData (pinPinnedPathData, sizeof (pinPinnedPathData));
    p4.applyTransform(juce::AffineTransform::rotation(3.14159));
    pinPinnedButton.reset (new ShapeButton ( "pinPinnedButton", Colours::white, Colours::lightgrey, Colours::white ));
    pinPinnedButton->setShape (p4, true, true, false);
    pinPinnedButton->setClickingTogglesState(true);
    pinPinnedButton->setTooltip("Stop keeping window on top");
    pinPinnedButton->addListener (this);
    addAndMakeVisible (pinPinnedButton.get());
    pinPinnedButton->setVisible(false);

    Path p5;
    p5.loadPathFromData (fullscreenActivatePathData, sizeof (fullscreenActivatePathData));
    p5.applyTransform(juce::AffineTransform::verticalFlip(0));
    fullscreenActivateButton.reset (new ShapeButton ( "fullscreenActivateButton", Colours::white, Colours::lightgrey, Colours::white ));
    fullscreenActivateButton->setShape (p5, true, true, false);
    fullscreenActivateButton->setClickingTogglesState(true);
    fullscreenActivateButton->setTooltip("Enter fullscreen");
    fullscreenActivateButton->addListener (this);
    addAndMakeVisible (fullscreenActivateButton.get());
    fullscreenActivateButton->setVisible(false);

    Path p6;
    p6.loadPathFromData (fullscreenDeactivatePathData, sizeof (fullscreenDeactivatePathData));
    p6.applyTransform(juce::AffineTransform::verticalFlip(0));
    fullscreenDeactivateButton.reset (new ShapeButton ( "fullscreenDeactivateButton", Colours::white, Colours::lightgrey, Colours::white ));
    fullscreenDeactivateButton->setShape (p6, true, true, false);
    fullscreenDeactivateButton->setClickingTogglesState(true);
    fullscreenDeactivateButton->setTooltip("Exit fullscreen");
    fullscreenDeactivateButton->addListener (this);
    addAndMakeVisible (fullscreenDeactivateButton.get());
    fullscreenDeactivateButton->setVisible(false);

    Path p7;
    p7.loadPathFromData (lightDarkModePathData, sizeof (lightDarkModePathData));
    p7.applyTransform(juce::AffineTransform::verticalFlip(0));
    lightDarkModeButton.reset (new ShapeButton ( "lightDarkModeButton", Colours::white, Colours::lightgrey, Colours::white ));
    lightDarkModeButton->setShape (p7, true, true, false);
    lightDarkModeButton->setTooltip("Light/dark mode");
    lightDarkModeButton->addListener (this);
    addAndMakeVisible (lightDarkModeButton.get());
    lightDarkModeButton->setVisible(false);

    Path p8;
    p8.loadPathFromData (fontPathData, sizeof (fontPathData));
    p8.applyTransform(juce::AffineTransform::verticalFlip(0));
    fontButton.reset (new ShapeButton ( "fontButton", Colours::white, Colours::lightgrey, Colours::white ));
    fontButton->setShape (p8, true, true, false);
    fontButton->setTooltip("Show/hide font settings");
    fontButton->addListener (this);
    addAndMakeVisible (fontButton.get());
    fontButton->setVisible(false);

    Path p9;
    p9.loadPathFromData (columnsTwoPathData, sizeof (columnsTwoPathData));
    p9.applyTransform(juce::AffineTransform::verticalFlip(0));
    columnsTwoButton.reset (new ShapeButton ( "colummnsTwoButton", Colours::white, Colours::lightgrey, Colours::white ));
    columnsTwoButton->setShape (p9, true, true, false);
    columnsTwoButton->setTooltip("Display two pages");
    columnsTwoButton->addListener (this);
    addAndMakeVisible (columnsTwoButton.get());
    columnsTwoButton->setVisible(false);

    Path p10;
    p10.loadPathFromData (columnsOnePathData, sizeof (columnsOnePathData));
    p10.applyTransform(juce::AffineTransform::verticalFlip(0));
    columnsOneButton.reset (new ShapeButton ( "colummnsOneButton", Colours::white, Colours::lightgrey, Colours::white ));
    columnsOneButton->setShape (p10, true, true, false);
    columnsOneButton->setTooltip("Display single page");
    columnsOneButton->addListener (this);
    addAndMakeVisible (columnsOneButton.get());
    columnsOneButton->setVisible(false);

    Path p11;
    p11.loadPathFromData (fitWidthPathData, sizeof (fitWidthPathData));
    fitWidthButton.reset (new ShapeButton ( "fitWidthButton", Colours::white, Colours::lightgrey, Colours::white ));
    fitWidthButton->setShape (p11, true, true, false);
    fitWidthButton->setTooltip("Fit to window width");
    fitWidthButton->addListener (this);
    addAndMakeVisible (fitWidthButton.get());
    fitWidthButton->setVisible(false);   

    Path p12;
    p12.loadPathFromData (fitHeightPathData, sizeof (fitHeightPathData));
    fitHeightButton.reset (new ShapeButton ( "fitHeightButton", Colours::white, Colours::lightgrey, Colours::white ));
    fitHeightButton->setShape (p12, true, true, false);
    fitHeightButton->setTooltip("Fit to window height");
    fitHeightButton->addListener (this);
    addAndMakeVisible (fitHeightButton.get());
    fitHeightButton->setVisible(false);

    Path p13;
    p13.loadPathFromData (closePathData, sizeof (closePathData));
    closeButton.reset (new ShapeButton ( "closeButton", Colours::white, Colours::lightgrey, Colours::white ));
    closeButton->setShape (p13, true, true, false);
    closeButton->setTooltip("Close preferences");
    closeButton->addListener (this);
    preferencesContainer.addAndMakeVisible (closeButton.get());

/*
    btnCurrent.reset (new TextButton ("btnCurrent"));
    btnCurrent->setLookAndFeel(minimalistSongLnF);
    btnCurrent->setButtonText ("");
    btnCurrent->addListener (this);   

    btnPrev.reset (new TextButton ("btnPrev"));
    btnPrev->setLookAndFeel(minimalistSongLnF);
    btnPrev->setButtonText ("");
    btnPrev->addListener (this);   

    btnNext.reset (new TextButton ("btnNext"));
    btnNext->setLookAndFeel(minimalistSongLnF);
    btnNext->setButtonText ("");
    btnNext->addListener (this);   

    btnModeSwitch.reset (new TextButton ("btnModeSwitch"));
    btnModeSwitch->setLookAndFeel(blankLnF);
    btnModeSwitch->setButtonText ("");
    //btnModeSwitch->setTooltip("Switch between rackspaces and songs");
    //btnModeSwitch->addListener (this);  
    //addAndMakeVisible (btnModeSwitch.get());
*/
    setlistButton.reset (new TextButton ("All Songs"));
    setlistButton->setLookAndFeel(setlistButtonLnF);
    setlistButton->setButtonText ("All Songs");
    setlistButton->addListener (this);   

    fontUp.reset (new TextButton ("+"));
    fontUp->setLookAndFeel(popOverLnf);
    fontUp->setButtonText ("+");
    fontUp->addListener (this);   

    fontDown.reset (new TextButton ("-"));
    fontDown->setLookAndFeel(popOverLnf);
    fontDown->setButtonText ("-");
    fontDown->addListener (this);   

    fontMono.reset (new TextButton ("Mono"));
    fontMono->setLookAndFeel(popOverLnf);
    fontMono->setButtonText ("Mono");
    fontMono->setClickingTogglesState(true);
    fontMono->addListener (this);  

    fontPopOverLabel.reset (new Label ("Font","Font"));
    fontPopOverLabel->setLookAndFeel(popOverLabelLnf);

    fontButtonContainer.addAndMakeVisible(fontUp.get());
    fontButtonContainer.addAndMakeVisible(fontDown.get());
    fontButtonContainer.addAndMakeVisible(fontMono.get());
    fontButtonContainer.addAndMakeVisible(fontPopOverLabel.get());

    createInvertedImage.reset (new TextButton ("Create"));
    createInvertedImage->setLookAndFeel(popOverLnf);
    createInvertedImage->setButtonText ("Create");
    createInvertedImage->addListener (this);  

    missingImageLabel.reset (new Label ("missingImageLabel",CP_NO_DARK_MODE_IMAGES));
    missingImageLabel->setLookAndFeel(popOverLabelLnf);

    missingImageContainer.addAndMakeVisible(createInvertedImage.get());
    missingImageContainer.addAndMakeVisible(missingImageLabel.get());

    for (int i = 0; i < DEFAULT_RACKSPACES_SONGS; ++i) {
        std::string number = std::to_string(i);
        std::string name = std::string(2 - number.length(), '0') + std::to_string(i); // Pad with leading zero
        auto button = new TextButton(name); 
        buttons.add(button);
        buttons[i]->setClickingTogglesState(true);
        buttons[i]->setRadioGroupId(1);
        buttons[i]->getProperties().set("index", i);
        buttons[i]->getProperties().set("displayIndex", i + 1); // Default to non zero-based
        buttons[i]->getProperties().set("type", "button"); 
        buttons[i]->setTriggeredOnMouseDown(true);
        buttons[i]->addListener(this);  
    }
    buttons[0]->setToggleState (true, dontSendNotification);

    for (int i = 0; i < DEFAULT_VARIATIONS_SONGPARTS; ++i) {
        std::string number = std::to_string(i);
        auto button = new TextButton(number); 
        subButtons.add(button);
        subButtons[i]->setLookAndFeel(subButtonsLnF);
        subButtons[i]->setClickingTogglesState(true);
        subButtons[i]->setRadioGroupId(2);
        subButtons[i]->getProperties().set("index", i);
        subButtons[i]->getProperties().set("displayIndex", i + 1); // Default to non zero-based
        subButtons[i]->getProperties().set("type", "subButton"); 
        subButtons[i]->setTriggeredOnMouseDown(true);
        subButtons[i]->addListener(this);  
    }
    subButtons[0]->setToggleState (true, dontSendNotification);
    container.addAndMakeVisible(highlight.get());

    for (int i = 0; i < buttons.size(); ++i) {
        container.addAndMakeVisible(buttons[i]);
        buttons[i]->setVisible(false);
    }

    for (int i = 0; i < subButtons.size(); ++i) {
        container.addAndMakeVisible(subButtons[i]);
        subButtons[i]->setVisible(false);
    }
    
    // Setlists
    for (int i = 0; i < DEFAULT_SETLISTS; ++i) {
        std::string number = std::to_string(i);
        auto button = new TextButton(number); 
        setlistButtons.add(button);
        setlistButtons[i]->setLookAndFeel(setlistListButtonLnF);
        setlistButtons[i]->setClickingTogglesState(true);
        setlistButtons[i]->setRadioGroupId(3);
        setlistButtons[i]->setTriggeredOnMouseDown(true);
        setlistButtons[i]->getProperties().set("type", "setlistButton"); 
        setlistButtons[i]->addListener(this);  
        setlistContainer.addChildComponent(setlistButtons[i]);
    }

    // ChordPro
    for (auto i = 0; i < 100; ++i) {
        std::string number = std::to_string(i);
        auto label = new Label(number, number); 
        chordProLines.add(label);
        chordProLines[i]->setEditable (false, false, false);
        chordProLines[i]->setLookAndFeel(chordProLnF);
        chordProLines[i]->setVisible(true);
        chordProLines[i]->setInterceptsMouseClicks(false, false);
        chordProContainer.addAndMakeVisible(chordProLines[i]);
    }

    // ChordPro Images
    for ( auto i = 0; i < CP_DEFAULT_IMAGES; ++i) {
        auto imageComponent = new ImageComponent(std::to_string(i));
        chordProImages.add(imageComponent);
        chordProContainer.addAndMakeVisible(chordProImages[i]);
    }

    noSongsLabel.reset (new Label ("noSongs", "No Songs"));
    noSongsLabel->setEditable (false, false, false);
    noSongsLabel->setBounds (0, 0, getWidth(), headerHeight);
    noSongsLabel->setLookAndFeel(noSongLabelLnF);

    noChordProLabel.reset (new Label ("noSongs", NO_CHORDPRO_MESSAGE));
    noChordProLabel->setEditable (false, false, false);
    noChordProLabel->setBounds (0, 0, getWidth(), headerHeight);
    noChordProLabel->setLookAndFeel(noChordProLabelLnF);

    draggableResizer.addMouseListener(this, true);
    draggableResizer.setMouseCursor(MouseCursor::LeftRightResizeCursor);
    //containerRight.addAndMakeVisible (btnCurrent.get());
    //containerRight.addAndMakeVisible (btnPrev.get());
    //containerRight.addAndMakeVisible (btnNext.get());
    viewport.setViewedComponent(&container, false);
    viewportRight.setViewedComponent(&containerRight, false);
    viewport.getVerticalScrollBar().setColour(ScrollBar::thumbColourId, Colour::fromString(BACKGROUND_COLOR));
    viewportRight.getVerticalScrollBar().setColour(ScrollBar::thumbColourId, Colour::fromString(BACKGROUND_COLOR));

    addAndMakeVisible(viewport);
    addAndMakeVisible(viewportRight);
    addAndMakeVisible(draggableResizer);
    addAndMakeVisible(fontButtonContainer);
    addAndMakeVisible(missingImageContainer);
    addAndMakeVisible(setlistButton.get());
    addChildComponent(setlistContainer);
    addChildComponent(noSongsLabel.get());
    addChildComponent(preferencesContainer);
    containerRight.addChildComponent(noChordProLabel.get());
    //preferencesContainer.addAndMakeVisible(prefToggle.get());


    extensionWindow.reset(new MyDocumentWindow());
    extensionWindow->setContentNonOwned(this, true);
    extensionWindow->setResizable(true, true);
    extensionWindow->setUsingNativeTitleBar(true);

    setSize (Rectangle<int>::fromString(DEFAULT_WINDOW_POSITION).getWidth(), 
             Rectangle<int>::fromString(DEFAULT_WINDOW_POSITION).getHeight()
            );
            
    #if JUCE_MAC
        extensionWindow->setResizeLimits(180, 250, 10000, 10000);
    #else
        extensionWindow->getPeer()->setIcon(getWindowIcon());
        extensionWindow->setResizeLimits(200, 250, 10000, 10000);
    #endif

    String imageBase64 = MENU_ICON;
    MemoryOutputStream mo;
    auto result = Base64::convertFromBase64(mo, imageBase64);
    if (result) {
        menuIcon = ImageFileFormat::loadFrom(mo.getData(), mo.getDataSize());
    }
    menuIconComponent.setImage(menuIcon);
    menuIconComponent.setSize(16,16);
    addAndMakeVisible(menuIconComponent);
    menuIconComponent.setInterceptsMouseClicks(false,false);

    //if (extension == nullptr) ExtensionWindow::initialize();
}

ExtensionWindow::~ExtensionWindow()
{
    clockTimer.stopTimer();
    refreshTimer.stopTimer();
}

void ExtensionWindow::paint (Graphics& g)
{
    g.fillAll (Colour::fromString(BACKGROUND_COLOR));
    g.drawImageAt(menuIcon, 5,5);
}

void ExtensionWindow::setWindowPositionAndSize(int x, int y, int w, int h) {
    extension->extensionWindow->setTopLeftPosition (x, y);
    extension->extensionWindow->setSize(w, h);
    extension->resized();
}

Rectangle<int> ExtensionWindow::getWindowPositionAndSize() {
    return extension->extensionWindow->getBounds();
}

Image ExtensionWindow::getWindowIcon() {
    Image img;
    String imageBase64 = WINDOW_ICON;
    MemoryOutputStream mo;
    auto result = Base64::convertFromBase64(mo, imageBase64);
    if (result) {
        img = ImageFileFormat::loadFrom(mo.getData(), mo.getDataSize());
    }
    return img;
}

void ExtensionWindow::resized()
{
    windowTimer.stopTimer();
    windowTimer.startTimer(TIMER_WINDOW_STATE);
    int minWindowWidth = displayRightPanel ? 0 : 180;
    int minButtonHeight = 50;
    int largeScrollAreaWidth = 50;
    Point<int> viewPos = viewport.getViewPosition();
    Point<int> viewRightPos = viewportRight.getViewPosition();
    int columns = 1;
    int buttonHeightRatio = 5; // Ratio of width:height
    auto bounds = container.getBounds();
    bool largeScrollArea = preferences->getProperty("LargeScrollArea");
    auto buttonSize = largeScrollArea ? bounds.getWidth() - largeScrollAreaWidth 
                                      : bounds.getWidth();
    int buttonHeight = ((int)(buttonSize/buttonHeightRatio) < minButtonHeight) ? minButtonHeight 
                                                                               : (int)(buttonSize / buttonHeightRatio);
    float padding = buttonHeight * 0.1;
    float rowHeight = 0.0;
    auto x = displayRightPanel ? draggableResizer.getX() 
                               : getWidth();
    
    // The increased width of the pane separator/resizer is to help with touchscreens. The actual displayed width is overridden in the Paint method of MyDraggableComponent.
    draggableResizer.setBounds (juce::jmax(minWindowWidth, x), headerHeight, PANE_SEPARATOR_WIDTH, getHeight()); 
    int buttonDisplayCount = 0;
    for (int i = 0; i < buttons.size(); ++i) {
        if (buttons[i]->isVisible()) ++buttonDisplayCount;
    }

    int subButtonDisplayCount = 0;
    for (int i = 0; i < subButtons.size(); ++i) {
        if (subButtons[i]->isVisible()) ++subButtonDisplayCount;
    }

    // Don't display if only 1 sub button (except if not switching immediately)
    bool switchRackSongImmediately = true; //preferences->getProperty("SwitchImmediately");
    if (subButtonDisplayCount == 1 && switchRackSongImmediately) {
        subButtonDisplayCount = 0;
        subButtons[0]->setVisible(false);
        highlight->setVisible(false);
    } else {
        highlight->setVisible(true);
    }

    // Calculate how many rows to display (based on column count)
    auto rows = (buttonDisplayCount + subButtonDisplayCount) / columns;
    rows = rows + 0.5 - (rows < 0); 
    int rowCount = (int)rows;

    String headerLabel = header->getText();
    Font headerLabelFont = header->getFont();
    int headerLabelWidth = headerLabelFont.getStringWidth(headerLabel);
    header->setBounds (0, 0, getWidth(), headerHeight);
    clock->setBounds (getWidth()/2 - 50, 0, 100, headerHeight);
    clock->setVisible(getWidth() > 460 && clock->getX() > headerLabelWidth ? true : false);
    if (chordProImagesOnly && getWidth() < 560) clock->setVisible(false);
    if (viewportRight.getWidth() <= 305 && viewportRight.isVisible()) fontButtonContainer.setVisible(false);
    if (viewportRight.getWidth() <= 375 && viewportRight.isVisible()) missingImageContainer.setVisible(false);
    
    //auto r = header->getBounds();
    int iconX = header->getBounds().getWidth() - 60;
    int iconY = header->getBounds().getY();
    int iconW = 60;
    int iconH = header->getHeight();
    auto iconBounds = Rectangle(iconX, iconY, iconW, iconH);

    if (pinUnpinnedButton->isVisible()) {
        //pinUnpinnedButton->setBounds (r.removeFromRight (60).withSizeKeepingCentre (25, 25));
        pinUnpinnedButton->setBounds(iconBounds.withSizeKeepingCentre(25,25));
    } else {
        //r.setY(r.getY() + 5);
        //pinPinnedButton->setBounds (r.removeFromRight (60).withSizeKeepingCentre (13, 13));
        //r.setY(r.getY() - 5);
        pinPinnedButton->setBounds(iconBounds.withY(5).withSizeKeepingCentre(13,13));
    }
    iconBounds = iconBounds.withX(iconBounds.getX() - 40);
    if (displayRightPanel) {
        if (fullscreenActivateButton->isVisible()) {
            //fullscreenActivateButton->setBounds(r.removeFromRight(40).withSizeKeepingCentre (25, 25));
            fullscreenActivateButton->setBounds(iconBounds.withSizeKeepingCentre(25,25));
        } else {
            //fullscreenDeactivateButton->setBounds(r.removeFromRight(40).withSizeKeepingCentre (25, 25));
            fullscreenDeactivateButton->setBounds(iconBounds.withSizeKeepingCentre(25,25));
        }
        //lightDarkModeButton->setBounds (r.removeFromRight (45).withSizeKeepingCentre (27, 27));
        
        if (chordProForCurrentSong) {
            iconBounds = iconBounds.withX(iconBounds.getX() - 45);
            lightDarkModeButton->setBounds(iconBounds.withSizeKeepingCentre(27,27));
            lightDarkModeButton->setVisible(chordProForCurrentSong && lightDarkModeButton->getX() > headerLabelWidth + 20);
        }
        if (!chordProImagesOnly && chordProForCurrentSong) {
            iconBounds = iconBounds.withX(iconBounds.getX() - 45);
            //fontButton->setBounds (r.removeFromRight (45).withSizeKeepingCentre (27, 27));
            fontButton->setBounds (iconBounds.withSizeKeepingCentre(27,27));
            fontButton->setVisible(fontButton->getX() > headerLabelWidth + 20);
        }
        if (chordProImagesOnly && chordProForCurrentSong) {
            iconBounds = iconBounds.withX(iconBounds.getX() - 45);
            if (chordProTwoColumns) {
                //columnsOneButton->setBounds (r.removeFromRight (45).withSizeKeepingCentre (27, 27));
                columnsOneButton->setBounds (iconBounds.withSizeKeepingCentre(27,27));
                columnsOneButton->setVisible(chordProImagesOnly && columnsOneButton->getX() > headerLabelWidth + 20);
            } else {
                //columnsTwoButton->setBounds (r.removeFromRight (45).withSizeKeepingCentre (27, 27));
                columnsTwoButton->setBounds (iconBounds.withSizeKeepingCentre(27,27));
                columnsTwoButton->setVisible(chordProImagesOnly && columnsTwoButton->getX() > headerLabelWidth + 20);
            }
            iconBounds = iconBounds.withX(iconBounds.getX() - 45);
            if (fitHeight) {
                //fitWidthButton->setBounds (r.removeFromRight (45).withSizeKeepingCentre (27, 27));
                fitWidthButton->setBounds(iconBounds.withSizeKeepingCentre(27,27));
                fitWidthButton->setVisible(chordProImagesOnly && fitWidthButton->getX() > headerLabelWidth + 20);
            } else {
                //fitHeightButton->setBounds (r.removeFromRight (45).withSizeKeepingCentre (27, 27));
                fitHeightButton->setBounds(iconBounds.withSizeKeepingCentre(27,27));
                fitHeightButton->setVisible(chordProImagesOnly && fitHeightButton->getX() > headerLabelWidth + 20);
            }
        }
    }
    iconBounds = iconBounds.withX(iconBounds.getX() - 45);
    if (sidePanelOpenButton->isVisible()) {
        //sidePanelOpenButton->setBounds (r.removeFromRight (45).withSizeKeepingCentre (27, 27));
        sidePanelOpenButton->setBounds (iconBounds.withSizeKeepingCentre(27,27));
    } else {
        //sidePanelCloseButton->setBounds (r.removeFromRight (45).withSizeKeepingCentre (27, 27));
        sidePanelCloseButton->setBounds(iconBounds.withSizeKeepingCentre(27,27));
    }

    int scrollbarBuffer = 2;
    int selectedButton = 999;
    for (int i = 0; i < buttons.size(); ++i) {
         buttons[i]->setBounds (buttonSize * (i % columns) + padding,
                                       buttonHeight * (i / columns) + padding + (i > selectedButton ? buttonHeight * subButtonDisplayCount : 0),
                                       buttonSize - padding - scrollbarBuffer,
                                       buttonHeight - padding);
        if (buttons[i]->getToggleState()) {  // Display sub buttons
            selectedButton = i;
            for (int j = 0; j < subButtonDisplayCount; ++j) {
               subButtons[j]->setBounds (buttonSize * (j % columns) + padding,
                                       buttonHeight * ((j+1) / columns) + (buttonHeight * (i / columns) + padding),
                                       buttonSize - padding - scrollbarBuffer,
                                       buttonHeight - padding);
            }
            highlight->setBounds (buttonSize * (i % columns) + padding,
                                    buttonHeight + (buttonHeight * (i / columns) + padding),
                                    buttonSize - padding - scrollbarBuffer,
                                    buttonHeight * subButtonDisplayCount - padding
            );
            highlight->getProperties().set("buttonHeight", buttonHeight - padding);
        }
    }

    container.setBounds(0, 50, juce::jmax (minWindowWidth-10, x - 10), headerHeight + (buttonHeight * rowCount) + padding);
    containerRight.setBounds(juce::jmax (minWindowWidth-10, x - 10), 50, getWidth()- juce::jmax (minWindowWidth, x), getHeight() - headerHeight);
    fontButtonContainer.setBounds(getWidth() - 300, headerHeight, 290, headerHeight);
    missingImageContainer.setBounds(getWidth() - 370, headerHeight, 360, headerHeight);
    noSongsLabel->setBounds(0, headerHeight * 4, x, 100);
    noChordProLabel->setBounds(0, headerHeight * 4, getWidth() - x, 100);


    viewport.setBounds(0, headerHeight * 2, juce::jmax (minWindowWidth, x), getHeight() - headerHeight);
    viewport.setViewPosition(viewPos);
    viewportRight.setBounds(juce::jmax (minWindowWidth, x), headerHeight, getWidth() - juce::jmax (minWindowWidth, x), getHeight() - headerHeight);

    //btnCurrent->setBounds (0 , containerRight.getHeight()/4, containerRight.getWidth(), containerRight.getHeight()/2);
    //btnPrev->setBounds (10 , 10, containerRight.getWidth()-10, containerRight.getHeight()/4);
    //btnNext->setBounds (10 , containerRight.getHeight()*3/4, containerRight.getWidth()-10, containerRight.getHeight()/4);
    //btnModeSwitch->setBounds (15, 0, getWidth() > 230 ? 120 : 60, headerHeight);

    setlistButton->setBounds(0, headerHeight, juce::jmax (minWindowWidth, x), headerHeight);
    menuIconComponent.setBounds(x - 30, headerHeight * 1.5 - 6, 8, 12);

    fontDown->setBounds (80,5,50,headerHeight-10);
    fontUp->setBounds (140,5,50,headerHeight-10);
    fontMono->setBounds (200,5,80,headerHeight-10);
    fontPopOverLabel->setBounds (0, 0, 80, headerHeight);

    missingImageLabel->setBounds (0, 0, 260, headerHeight);
    createInvertedImage->setBounds (260, 5, 90, headerHeight-10);

    preferencesContainer.setBounds(x, headerHeight, getWidth() - x, getHeight());
    closeButton->setBounds(getWidth() - x - 70, 20, 30, 30 );
    //prefToggle->setBounds(50, 400, 50, 50);
    for (int i = 0; i < 4; ++i) {
        prefButtons[i]->setBounds(50, 148 + 35 * i, 40, 40);
    }
    prefButtons[4]->setBounds(50, 353, 40, 40);
    prefButtons[5]->setBounds(50, 388, 40, 40);

    for (int i = 0; i < prefColorLabels.size(); ++i) {
        prefColorLabels[i]->setBounds(50, 440 + 35 * i, 80, 30);
    }
    //draggableResizer.setVisible(displayRightPanel);
    //viewportRight.setVisible(displayRightPanel);
  
    int setlistCount = 0;
    /*
    int setlistDisplayCount = 0;
    for (int i = 0; i < setlistButtons.size(); ++i) {
        if (setlistButtons[i]->isVisible()) ++setlistDisplayCount;
    }
    */
    setlistContainer.setBounds(0, headerHeight * 2, x, getHeight()); //buttonHeight * setlistDisplayCount);
    for (int i = 0; i < setlistButtons.size(); ++i) {
        if (setlistButtons[i]->isVisible()) {
            setlistButtons[i]->setBounds(0, (buttonHeight * setlistCount), x, buttonHeight);
            setlistCount++;
        }
    }

    if (chordProForCurrentSong && viewportRight.isVisible()) {
        float runningHeight = 0.0;
        float columns = (chordProTwoColumns && chordProImagesOnly) ? 2.0 : 1.0;
        float padding = chordProImagesOnly ? chordProContainer.getWidth() * 0.005 : 0.0;
        int imageCount = chordProGetVisibleImageCount();
        if (!chordProImagesOnly) runningHeight += chordProTopPadding;

        for (auto i = 0; i < chordProLines.size(); ++i) {
            if (chordProLines[i]->isVisible() && !(chordProImagesOnly && chordProLines[i]->getProperties()["type"] == "")) {
                if (chordProLines[i]->getProperties()["type"] == "chordAndLyrics") {
                    rowHeight = 80.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "title") {
                    rowHeight = 80.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "subtitle") {
                    rowHeight = 50.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "label") {
                    rowHeight = 60.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "comment") {
                    rowHeight = 50.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "tab") {
                    rowHeight = 30.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "grid") {
                    rowHeight = 40.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "image") {
                    rowHeight = 0.0;
                    int imageIndex = chordProLines[i]->getProperties()["imageIndex"]; 
                    if (extension->chordProImages[imageIndex]->isVisible()) {
                        float originalWidth = (float)(extension->chordProImages[imageIndex]->getImage().getWidth()); 
                        float originalHeight = (float)(extension->chordProImages[imageIndex]->getImage().getHeight());
                        float ratio = originalWidth / (float)(viewportRight.getWidth() - 10);
                        float newWidth = (float)(viewportRight.getWidth() - 10) / columns;
                        if (chordProTwoColumns && chordProImagesOnly) newWidth = newWidth - (padding / 2.0);
                        float newHeight = originalHeight / (ratio * columns);
                        int imageX = 0;
                        if (fitHeight && chordProImagesOnly) {
                            newHeight = (float)(viewportRight.getHeight() - padding);
                            newWidth = originalWidth / originalHeight * newHeight;
                            imageX = juce::jmax(0, (int)((viewportRight.getWidth() - newWidth) / 2));
                            if (chordProTwoColumns) imageX = juce::jmax(0, (int)((viewportRight.getWidth() - (newWidth * columns)) / 2));
                        }
                        rowHeight = newHeight; 
                        if (chordProTwoColumns && chordProImagesOnly & !fitHeight) {
                            extension->chordProImages[imageIndex]->setBounds((imageIndex % 2 == 0) ? 0 : newWidth + padding , runningHeight, truncatePositiveToUnsignedInt(newWidth), rowHeight);
                        } else if (chordProTwoColumns && chordProImagesOnly && fitHeight) {
                            extension->chordProImages[imageIndex]->setBounds((imageIndex % 2 == 0) ? imageX : imageX + newWidth + padding, runningHeight, truncatePositiveToUnsignedInt(newWidth), rowHeight);
                        } else {
                            extension->chordProImages[imageIndex]->setBounds(imageX, runningHeight, truncatePositiveToUnsignedInt(newWidth), rowHeight);
                        }
                        rowHeight = (chordProTwoColumns && chordProImagesOnly && imageIndex % 2 == 0 && imageIndex < imageCount - 1) ? 0 : rowHeight + padding;
                    }
                } else {
                    rowHeight = 40.0 * chordProFontSize;
                }
                chordProLines[i]->setBounds(10, runningHeight, chordProContainer.getWidth(), rowHeight);
                runningHeight += rowHeight;
            } else {
                chordProLines[i]->setBounds(10,runningHeight,chordProContainer.getWidth(),0);
            }
        }
        chordProContainer.setBounds(viewportRight.getX(), viewportRight.getY(), viewportRight.getWidth() - 10, runningHeight);
    }
    viewportRight.setViewPosition(viewRightPos);
}

void ExtensionWindow::refreshUI() {

    //if (!lib->inSetlistMode() && lockToSetlistMode) return;
    int songIndex = lib->inSetlistMode() ? lib->getCurrentSongIndex() : extension->getButtonSelected();
    int songPartIndex = lib->inSetlistMode() ? lib->getCurrentSongpartIndex() : 0;

    //extension->log("Refresh UI. Song: " + std::to_string(songIndex));
    // Reset all buttons
    for (int i = 0; i < extension->buttons.size(); ++i) {
        extension->buttons[i]->setToggleState(false, dontSendNotification);
        extension->buttons[i]->setVisible(false);
    }
    // Reset all sub buttons
    for (int i = 0; i < extension->subButtons.size(); ++i) {
        extension->subButtons[i]->setToggleState(false, dontSendNotification);
        extension->subButtons[i]->setVisible(false);
    }
    // Reset large previous/current/next titles
    /*
    extension->btnPrev->setButtonText("");
    extension->btnCurrent->setButtonText("");
    extension->btnNext->setButtonText("");
*/
    //if (lib->inSetlistMode()) {
            
            
            //updateButtonLabel(SONG_TITLE);
            setTitleBarName(SONG_WINDOW_TITLE);
            updateButtonNames(lib->getSongNames());
            updateSubButtonNames(extension->getSubButtonNamesByIndex(songIndex));
            selectButton(songIndex);
            selectSubButton(songPartIndex);//lib->getCurrentSongpartIndex());
            chordProReadFile(songIndex);
   
   /* }  else {
            int rackspaceIndex = lib->getCurrentRackspaceIndex();
            updateButtonLabel(RACKSPACE_TITLE);
            setTitleBarName(RACKSPACE_WINDOW_TITLE);
            updateButtonNames(lib->getRackspaceNames());
            updateSubButtonNames(lib->getVariationNames(rackspaceIndex));
            selectButton(rackspaceIndex);
            selectSubButton(lib->getCurrentVariationIndex());
            extension->chordProForCurrentSong = false;
            extension->chordProReset();
            extension->chordProDisplayGUI(false);
    }
    */
    extension->resized();
}

void ExtensionWindow::setTitleBarName(const String& name) {
    extension->extensionWindow->setName(name);
}

void ExtensionWindow::setZeroBasedNumbering(bool zeroBased) {
    extension->preferences->setProperty("ZeroBasedNumbers", zeroBased); 
    int offset = zeroBased ? 0 : 1;
    for (int i = 0; i < extension->buttons.size(); ++i) {
        extension->buttons[i]->getProperties().set("displayIndex", i + offset);
    }
    extension->resized();
}

void ExtensionWindow::setImmediateSwitching(bool immediateSwitch) {
    extension->preferences->setProperty("ImmediateSwitching", immediateSwitch);     
}

void ExtensionWindow::setDarkMode(bool darkMode) {
    extension->preferences->setProperty("DarkMode", darkMode); 
    extension->chordProDarkMode = darkMode;
    //extension->chordProRefresh();
    //extension->repaint();
}

void ExtensionWindow::setDisplayVariationNames(bool display) {
    extension->preferences->setProperty("DisplayVariationNames", display); 
    displayVariationsForSong = display;
    //refreshUI();
}

void ExtensionWindow::setFitToPageHeight(bool fitToHeight) {
    extension->preferences->setProperty("FitToPageHeight", fitToHeight); 
    extension->fitHeight = fitToHeight;
    extension->fitWidthButton->setVisible(fitToHeight);
    extension->fitHeightButton->setVisible(!fitToHeight);
    extension->resized();
}

void ExtensionWindow::setDisplayTwoPages(bool display) {
    extension->preferences->setProperty("DisplayTwoPages", display); 
    extension->chordProTwoColumns = display;
    extension->columnsOneButton->setVisible(display);
    extension->columnsTwoButton->setVisible(!display);
    extension->resized();
}

void ExtensionWindow::chordProSetLeftMarginLabels(bool onLeft) {
    extension->preferences->setProperty("LeftMarginLabels", onLeft); 
    chordProLeftLabels = onLeft;
    //refreshUI();
}

void ExtensionWindow::toggleZeroBasedNumbering() {
    bool status = extension->preferences->getProperty("ZeroBasedNumbers");
    extension->preferences->setProperty("ZeroBasedNumbers", !status); 
    setZeroBasedNumbering(!status);
}

void ExtensionWindow::toggleImmediateSwitching() {
    bool status = extension->preferences->getProperty("ImmediateSwitching");
    extension->preferences->setProperty("ImmediateSwitching", !status);     
}

void ExtensionWindow::setLargeScrollArea(bool largeScrollArea) {
    extension->preferences->setProperty("LargeScrollArea", largeScrollArea); 
    extension->resized();    
}

void ExtensionWindow::toggleLargeScrollArea() {
    bool status = extension->preferences->getProperty("LargeScrollArea");
    extension->preferences->setProperty("LargeScrollArea", !status); 
    extension->resized();   
}

void ExtensionWindow::toggleThickBorders() {
    bool status = extension->preferences->getProperty("ThickBorders");
    extension->preferences->setProperty("ThickBorders", !status); 
    refreshUI();
}

void ExtensionWindow::toggleLockToSetlistMode() {
    lockToSetlistMode = !lockToSetlistMode;
    if (!lib->inSetlistMode() && lockToSetlistMode) {
        lib->switchToSetlistView();
    } else if (!lib->inSetlistMode() && !lockToSetlistMode) {
        lib->switchToPanelView();
    }
}

void ExtensionWindow::toggleVariationsInSetlistMode() {
    bool status = extension->preferences->getProperty("DisplayVariationNames");
    extension->preferences->setProperty("DisplayVariationNames", !status); 
    displayVariationsForSong = !status;
    refreshUI();
}

void ExtensionWindow::toggleLeftMarginLabels() {
    bool status = extension->preferences->getProperty("LeftMarginLabels");
    extension->preferences->setProperty("LeftMarginLabels", !status);
    chordProLeftLabels = !status;
    refreshUI();
}

void ExtensionWindow::toggleDarkMode() {
    bool status = extension->preferences->getProperty("DarkMode");
    extension->preferences->setProperty("DarkMode", !status); 
    extension->chordProDarkMode = !status;
    extension->chordProRefresh();
    extension->repaint();
}

void ExtensionWindow::toggleSmallChordFont() {
    bool status = extension->preferences->getProperty("SmallChordFont");
    extension->preferences->setProperty("SmallChordFont", !status);
    chordProSmallChordFont = !status;
    refreshUI();
}

/*
void ExtensionWindow::displayRackspaceVariationInSetlistMode(bool display) {
    extension->preferences->setProperty("RackspaceVariationInSetlistMode", display); 
    extension->resized();    
}

void ExtensionWindow::toggleRackspaceVariationInSetlistMode() {
    bool status = extension->preferences->getProperty("RackspaceVariationInSetlistMode");
    extension->preferences->setProperty("RackspaceVariationInSetlistMode", !status); 
    extension->resized();   
}
*/

String ExtensionWindow::buttonName(int index) {
    String name = "";
    if (index < buttons.size()) {
        name = extension->buttons[index]->getButtonText();
    }
    return name;
}

bool ExtensionWindow::isButtonSelected(int index) {
    if (index < extension->subButtons.size() && index >= 0) {
        return extension->buttons[index]->getToggleState();
    }
    return false;
}

int ExtensionWindow::getButtonSelected() {
    int selected = 0;
    for (int i = 0; i < extension->buttons.size(); ++i) {
        if (extension->buttons[i]->getToggleState()) {
            selected = i;
            break;
        }
    }
    return selected;
}

int ExtensionWindow::getVisibleButtonCount() {
    int visible = 0;
    for (auto i = 0; i < extension->buttons.size(); ++i) {
        if (extension->buttons[i]->isVisible()) {
            ++visible;
        }
    }
    return visible;
}

int ExtensionWindow::getVisibleSubButtonCount() {
    int visible = 0;
    for (auto i = 0; i < extension->subButtons.size(); ++i) {
        if (extension->subButtons[i]->isVisible()) {
            ++visible;
        }
    }
    return visible;
}

void ExtensionWindow::selectSetlistButton(int index) {
    extension->setlistButtons[index]->setToggleState(true, dontSendNotification);
}

void ExtensionWindow::selectButton(int index) {
    //if (!lib->inSetlistMode() && lockToSetlistMode) return;
    //extension->log("Select button " + std::to_string(index));
    if (index < extension->buttons.size() && index >= 0) {
        extension->buttons[index]->setToggleState(true, dontSendNotification);
        Rectangle<int> viewportBounds = extension->viewport.getViewArea();
        Rectangle<int> buttonBounds = extension->buttons[index]->getBounds();
        auto pad = buttonBounds.getWidth() / 40;
        pad = pad + 0.5 - (pad < 0); 
        int padding = (int)pad;
        int viewY = viewportBounds.getY();
        int btnY = buttonBounds.getY();
        if (btnY < viewY) {
            extension->viewport.setViewPosition(0, btnY - padding);
        }
        //updatePrevCurrNext(index);
        setSongLabel();
    }
}

void ExtensionWindow::selectSongForCurrentButton() {
    if (lib == nullptr) return;
        int buttonIndex = extension->getButtonSelected();
        if (!lib->inSetlistMode()) lib->switchToSetlistView();
        lib->switchToSong(buttonIndex, 0);
}

/*
void ExtensionWindow::updatePrevCurrNext(int index) {
    if (index < extension->buttons.size() && index >= 0) {
        String text = extension->buttons[index]->getButtonText();
        extension->btnCurrent->setButtonText(text);
        if (index > 0) {
            text = extension->buttons[index-1]->getButtonText();
            extension->btnPrev->setButtonText(text);
        } else {
            extension->btnPrev->setButtonText("");
        }
        if (index < extension->buttons.size()-1) {
            text = extension->buttons[index+1]->getButtonText();
            extension->btnNext->setButtonText(text);
        } else {
            extension->btnNext->setButtonText("");
        }
    }
}
*/

bool ExtensionWindow::isSubButtonSelected(int index) {
    if (index < extension->subButtons.size() && index >= 0) {
        return extension->subButtons[index]->getToggleState();
    }
    return false;
}

bool ExtensionWindow::isSubButtonsCollapsed() {
    bool collapsed = true;
    int buttonCount = extension->subButtons.size();
    if (buttonCount > 1) { // If only 1 button it will be collapsed by default
        for (int i = 0; i < buttonCount; ++i) {
            if (extension->subButtons[i]->isVisible()) {
                collapsed = false;
                break;
            }
        }
    }
    return collapsed;
}

void ExtensionWindow::selectSubButton(int index) {
    //if (!lib->inSetlistMode() && lockToSetlistMode) return;
    if (index < extension->subButtons.size() && index >= 0) {
        extension->subButtons[index]->setToggleState(true, dontSendNotification);
        updateViewportPositionForSubButtons();
    }
}

void ExtensionWindow::addButtons(int count) {
    int buttonCount = extension->buttons.size();
    int index;
    int offset = extension->preferences->getProperty("ZeroBasedNumbers") ? 0 : 1;
    for (auto i = 0; i < count; ++i) {
        index = buttonCount + i;
        std::string number = std::to_string(index);
        auto button = new TextButton(number); 
        extension->buttons.add(button);
        extension->buttons[index]->setLookAndFeel(extension->buttonsLnF);
        extension->buttons[index]->setClickingTogglesState(true);
        extension->buttons[index]->setRadioGroupId(1);
        extension->buttons[index]->getProperties().set("index", index);
        extension->buttons[index]->getProperties().set("displayIndex", index + offset);
        extension->buttons[index]->getProperties().set("type", "button"); 
        extension->buttons[index]->setTriggeredOnMouseDown(true);
        extension->buttons[index]->addListener(extension);  
        extension->container.addAndMakeVisible(extension->buttons[index]);
    }
}

void ExtensionWindow::updateButtonNames(std::vector<std::string> buttonNames) {
    //if (!lib->inSetlistMode() && lockToSetlistMode) return;
    int newButtonCount = buttonNames.size();
    int currentButtonCount = extension->buttons.size();
    bool border = extension->preferences->getProperty("ThickBorders");
    if (newButtonCount == 0) {
        extension->noSongsLabel->setVisible(true);
        //extension->log("No Songs");
    } else {
        extension->noSongsLabel->setVisible(false);
        //extension->log("Has Songs");
        if (newButtonCount > currentButtonCount) {
            addButtons(newButtonCount-currentButtonCount);
            currentButtonCount = newButtonCount;
        }
        for (auto i = 0; i < currentButtonCount; ++i) {
            if (i < newButtonCount) {
                extension->buttons[i]->setButtonText(buttonNames[i]);
                extension->buttons[i]->setVisible(true);
                extension->buttons[i]->getProperties().set("colour", DEFAULT_BUTTON_COLOR);
                extension->buttons[i]->getProperties().set("thickBorder", border);
            } else {
                extension->buttons[i]->setButtonText("");
                extension->buttons[i]->setVisible(false);
            }
        } 
    }
    extension->resized();
 }

void ExtensionWindow::compareButtonNames(std::vector<std::string> newButtonNames) {
    int buttonCount = extension->buttons.size();
    int newButtonCount = newButtonNames.size();
    int visibleButtons = getVisibleButtonCount();
    if (visibleButtons != newButtonCount) {
        refreshUI();
    } else {
        for (auto i = 0; i < newButtonCount; ++i) {
            if (i < buttonCount && newButtonNames[i] != extension->buttons[i]->getButtonText()) {
                refreshUI();
            }
        }
    }
}

void ExtensionWindow::addSubButtons(int count) {
    int buttonCount = extension->subButtons.size();
    int index;
    int offset = extension->preferences->getProperty("ZeroBasedNumbers") ? 0 : 1;
    for (auto i = 0; i < count; ++i) {
        index = buttonCount + i;
        std::string number = std::to_string(index);
        auto button = new TextButton(number); 
        extension->subButtons.add(button);
        extension->subButtons[index]->setLookAndFeel(extension->subButtonsLnF);
        extension->subButtons[index]->setClickingTogglesState(true);
        extension->subButtons[index]->setRadioGroupId(2);
        extension->subButtons[index]->getProperties().set("index", index);
        extension->subButtons[index]->getProperties().set("displayIndex", index + offset);
        extension->subButtons[index]->getProperties().set("type", "subButton"); 
        extension->subButtons[index]->setTriggeredOnMouseDown(true);
        extension->subButtons[index]->addListener(extension);  
        extension->container.addAndMakeVisible(extension->subButtons[index]);
    }
}

void ExtensionWindow::updateSubButtonNames(std::vector<std::string> buttonNames) {
    //if (!lib->inSetlistMode() && lockToSetlistMode) return;
    int newButtonCount = buttonNames.size();
    int currentButtonCount = extension->subButtons.size();
    bool border = extension->preferences->getProperty("ThickBorders");
    String borderColor = extension->preferences->getProperty("BorderColor");

    if (newButtonCount > currentButtonCount) {
        addSubButtons(newButtonCount-currentButtonCount);
        currentButtonCount = newButtonCount;
    }
    for (auto i = 0; i < currentButtonCount; ++i) {
        if (i < newButtonCount) {
            String name = buttonNames[i];
            String color = DEFAULT_SUBBUTTON_COLOR;
            extension->subButtons[i]->setButtonText(name);
            extension->subButtons[i]->getProperties().set("name", name);
            extension->subButtons[i]->setVisible(true);
            StringArray keys = extension->buttonColors.getAllKeys();
            for (int j = 0; j < keys.size(); ++j ) {
                if (name.contains(keys[j])) {
                    color = extension->buttonColors.getValue(keys[j],"");
                    if (extension->preferences->getProperty("RemoveColorKeywordFromName")) {
                        name = name.replace(keys[j], "");
                        name = name.replace("  ", " ");
                        extension->subButtons[i]->setButtonText(name);
                    }
                }
            }
            extension->subButtons[i]->getProperties().set("colour", color);
            extension->subButtons[i]->getProperties().set("thickBorder", border);
            extension->subButtons[i]->getProperties().set("borderColor", borderColor);
        } else {
            extension->subButtons[i]->setButtonText("");
            extension->subButtons[i]->setVisible(false);
            extension->subButtons[i]->getProperties().set("colour", DEFAULT_SUBBUTTON_COLOR);
            extension->subButtons[i]->getProperties().set("name", "");
        }
    }
    extension->resized();
 }

 void ExtensionWindow::compareSubButtonNames(std::vector<std::string> newButtonNames) {
    int buttonCount = extension->subButtons.size();
    int visibleButtons = getVisibleSubButtonCount();
    int newButtonCount = newButtonNames.size();
    if (visibleButtons > 0) { // Ignore if collapsed
        if (visibleButtons != newButtonCount) {
            refreshUI();
        } else {
            for (auto i = 0; i < newButtonCount; ++i) {
                if (i < buttonCount && newButtonNames[i] != extension->subButtons[i]->getProperties()["name"]) {
                    refreshUI();
                }
            }
        }
    }
}

std::vector<std::string> ExtensionWindow::getSubButtonNamesByIndex(int index) {
    std::vector<std::string> names;
    names = displayVariationsForSong ? lib->getVariationNamesForSong(index) : lib->getSongPartNames(index);
    /*
    if (lib->inSetlistMode()) {
        names = displayVariationsForSong ? lib->getVariationNamesForSong(index) : lib->getSongPartNames(index);
    } else {
        names = lib->getVariationNames(index);
    } 
    */
    return names;
} 

std::string ExtensionWindow::getSubButtonNameByIndex(int index, int subIndex) {
    std::string name;
    name = displayVariationsForSong ? lib->getVariationNameForSongPart(index, subIndex) : lib->getSongpartName(index, subIndex);
    /*
    if (lib->inSetlistMode()) {
        name = displayVariationsForSong ? lib->getVariationNameForSongPart(index, subIndex) : lib->getSongpartName(index, subIndex);
    } else {
        name = lib->getVariationName(index, subIndex);
    } 
    */
    return name;
} 

void ExtensionWindow::updateSetlistButtons(std::vector<std::string> buttonNames) {
    String name = lib->getSetlistName(lib->getCurrentSetlistIndex());
    extension->setlistButton->setButtonText(name);
    extension->setlistContainer.setVisible(false);

    int buttonCount  = buttonNames.size();
    for (auto i = 0; i < extension->setlistButtons.size(); ++i) {
        if (i < buttonCount) {
            extension->setlistButtons[i]->setButtonText(buttonNames[i]);
            extension->setlistButtons[i]->setVisible(true);
        } else {
            extension->setlistButtons[i]->setVisible(false);
        }
    }
}

bool ExtensionWindow::getDisplayVariationForSongPartStatus() {
    return displayVariationsForSong;
}

void ExtensionWindow::updateButtonLabel(const String& text) {
    if (text == "Songs"){ 
        extension->header->setLookAndFeel(extension->headerSongsLnF);
    } else {
        extension->header->setLookAndFeel(extension->headerRackspacesLnF);
    }
    extension->resized();
}

void ExtensionWindow::updateViewportPositionForSubButtons() {
    Rectangle<int> viewportBounds = extension->viewport.getViewArea();
    Rectangle<int> buttonBounds = extension->buttons[getButtonSelected()]->getBounds();
    auto pad = buttonBounds.getWidth() / 40;
    pad = pad + 0.5 - (pad < 0); 
    int padding = (int)pad;
    int viewY = viewportBounds.getY();
    int viewH = viewportBounds.getHeight();
    int btnH = buttonBounds.getHeight();
    int btnY = padding + (getButtonSelected() * (btnH + padding));
    int subButtonCount = getVisibleSubButtonCount();
    int totalH = (btnH + padding) * (subButtonCount + 2);
    if ((btnY + totalH) > (viewY + viewH)) {
        int adjY = (viewH - totalH > 0) ? (viewH - totalH) : 0;
        extension->viewport.setViewPosition(0, (btnY - adjY) > 0 ? (btnY - adjY) : 0);
    } 
}

void ExtensionWindow::displayExpandedWindow(bool display) {
    if (display) {
            extension->sidePanelCloseButton->setVisible(true);
            extension->sidePanelOpenButton->setVisible(false);
            extension->fullscreenActivateButton->setVisible(true);
            extension->clock->setVisible(true);
            if (lib->inSetlistMode() && extension->chordProForCurrentSong) {
                extension->chordProDisplayGUI(true);
            }
        } else {
            extension->sidePanelCloseButton->setVisible(false);
            extension->sidePanelOpenButton->setVisible(true);
            extension->fullscreenActivateButton->setVisible(false);
            extension->chordProDisplayGUI(false);
            extension->clock->setVisible(false);
            extension->header->setText(SONG_TITLE, dontSendNotification);
        }
        extension->resized();
}

void ExtensionWindow::buttonClicked (Button* buttonThatWasClicked)
{
   if (buttonThatWasClicked == sidePanelOpenButton.get() || buttonThatWasClicked == sidePanelCloseButton.get())
    {
        //displayRightPanel = !displayRightPanel;
        //setSize(container.getWidth() + (displayRightPanel ? 500 : 10), getHeight());
        //displayExpandedWindow(displayRightPanel);
        displaySongPanel = !displaySongPanel;
        setSongPanelPosition(displaySongPanel);

  /*  } else if (buttonThatWasClicked == btnPrev.get()) {
        if (lib->inSetlistMode()) {
            lib->switchToSong(juce::jmax(0, lib->getCurrentSongIndex()-1), 0);
        } else {
            lib->switchToRackspace(juce::jmax(0, lib->getCurrentRackspaceIndex()-1));
        }
    } else if (buttonThatWasClicked == btnNext.get()) {
        if (lib->inSetlistMode()) {
            lib->switchToSong(juce::jmin(buttons.size()-1,lib->getCurrentSongIndex()+1), 0);
        } else {
            lib->switchToRackspace(juce::jmin(buttons.size()-1,lib->getCurrentRackspaceIndex()+1));
        }
    */
    } else if (buttonThatWasClicked == pinUnpinnedButton.get() || buttonThatWasClicked == pinPinnedButton.get()) {
        bool newPinnedStatus = !(extension->extensionWindow->isAlwaysOnTop());
        pinUnpinnedButton->setVisible(!newPinnedStatus);
        pinPinnedButton->setVisible(newPinnedStatus);
        extension->extensionWindow->setAlwaysOnTop(newPinnedStatus);
        if (newPinnedStatus) {
            Rectangle<int> window = getWindowPositionAndSize();
            //log("Pinned window (x,y,w,h): " + std::to_string(window.getX()) + "," + std::to_string(window.getY()) + "," + std::to_string(window.getWidth()) + "," + std::to_string(window.getHeight()));
        }
        resized();
    } else if (buttonThatWasClicked == fullscreenActivateButton.get() || buttonThatWasClicked == fullscreenDeactivateButton.get()) {
        bool newFullscreenStatus = !extension->extensionWindow->isFullScreen();
        #if JUCE_WINDOWS
            newFullscreenStatus = !(Desktop::getInstance().getKioskModeComponent() == getTopLevelComponent());
            Desktop::getInstance().setKioskModeComponent(newFullscreenStatus ? getTopLevelComponent() : nullptr, false);
            if (!newFullscreenStatus) extension->extensionWindow->getPeer()->setIcon(getWindowIcon());
        #else
            extension->extensionWindow->setFullScreen(newFullscreenStatus);
        #endif

        fullscreenActivateButton->setVisible(!newFullscreenStatus);
        fullscreenDeactivateButton->setVisible(newFullscreenStatus);
        /*
        if (newFullscreenStatus) {
            sidePanelCloseButton->setColours(Colours::grey, Colours::grey, Colours::grey);
            sidePanelCloseButton->setTooltip("Disabled while in fullscreen");
            sidePanelCloseButton->removeListener(extension);
        } else {
            sidePanelCloseButton->setColours(Colours::white, Colours::lightgrey, Colours::white);
            sidePanelCloseButton->setTooltip("Close expanded window");
            sidePanelCloseButton->addListener(extension);
        }
        */
        resized();
    } else if (buttonThatWasClicked->getProperties()["type"] == "button") {
        bool switchRackSongImmediately = true; //preferences->getProperty("ImmediateSwitching");
        bool inSetlist = lib->inSetlistMode();
        int buttonIndex = buttonThatWasClicked->getProperties()["index"];
        int currentGPIndex = (inSetlist ? lib->getCurrentSongIndex() : -1);
        std::vector<std::string> blank;
        /*
        if (!inSetlist && lockToSetlistMode) {
            lib->switchToSetlistView();
            lib->switchToSong(buttonIndex, 0);
            return;
        }
        */

        // Ensure other buttons are deselected
        for (int i = 0; i < buttons.size(); ++i) {
            if (buttonIndex != i) {
                buttons[i]->setToggleState(false, dontSendNotification);
            }
        }
        if (isSubButtonsCollapsed() || (buttonIndex != prevButtonSelected)) {
            // Expand
            updateSubButtonNames(getSubButtonNamesByIndex(buttonIndex));
            updateViewportPositionForSubButtons();
            if (buttonIndex != currentGPIndex && switchRackSongImmediately) {
                selectSubButton(0); // Force selection of first variation/part to avoid sync issues
            }
             // Ensure no button is selected on new song/rackspace if pref is set
            if (buttonIndex != currentGPIndex && !switchRackSongImmediately) {
                for (int i = 0; i < subButtons.size(); ++i) {
                    if (subButtons[i]->getToggleState()) {
                        subButtons[i]->setToggleState(false, dontSendNotification);
                    }
                }
            }
        } else {         
            // Collapse
            updateSubButtonNames(blank);
        }
        if (buttonIndex != currentGPIndex && switchRackSongImmediately) {
            if (inSetlist) {
                lib->switchToSong(buttonIndex, 0);
            } else {
                //lib->switchToRackspace(buttonIndex, 0);
                //log("Button clicked " + std::to_string(buttonIndex));
                refreshUI();
            }
            //updatePrevCurrNext(buttonIndex);
            setSongLabel();
        }
        prevButtonSelected = buttonIndex;
    } else if (buttonThatWasClicked->getProperties()["type"] == "subButton") {
        bool switchRackSongImmediately = true; //preferences->getProperty("ImmediateSwitching");
        bool inSetlist = lib->inSetlistMode();
        int subButtonIndex = buttonThatWasClicked->getProperties()["index"];
        int buttonIndex = getButtonSelected();

        /*
        if (!inSetlist && lockToSetlistMode) {
            lib->switchToSetlistView();
            lib->switchToSong(buttonIndex, subButtonIndex);
            std::string songpartName = getSubButtonNameByIndex(buttonIndex, subButtonIndex);
            chordProScrollToSongPart(songpartName);
            return;
        }
        */
        if (inSetlist) {
            lib->switchToSong(buttonIndex, subButtonIndex);
            std::string songpartName = getSubButtonNameByIndex(buttonIndex, subButtonIndex);
            chordProScrollToSongPart(songpartName);
        } else {
            //lib->switchToRackspace(buttonIndex, subButtonIndex);
            std::string songpartName = getSubButtonNameByIndex(buttonIndex, subButtonIndex);
            chordProScrollToSongPart(songpartName);
        }
        // Ensure other buttons are toggled off
        for (int i = 0; i < subButtons.size(); ++i) {
            if (i != subButtonIndex && subButtons[i]->getToggleState()) {
                subButtons[i]->setToggleState(false, dontSendNotification);
            }
        }

        // Ensure large song/rackspace labels are in sync
        /*
        if (!switchRackSongImmediately) {
            updatePrevCurrNext(buttonIndex);
        }
        */
   // } else if (buttonThatWasClicked == btnModeSwitch.get()) {
     //   lib->inSetlistMode() ? lib->switchToPanelView() : lib->switchToSetlistView();
    } else if (buttonThatWasClicked == fontDown.get()) {
        chordProSetFontSize((chordProFontSize <= CP_MIN_FONT_SIZE) ? chordProFontSize : chordProFontSize - CP_FONT_STEP_SIZE);
        //chordProFontSize = (chordProFontSize <= CP_MIN_FONT_SIZE) ? chordProFontSize : chordProFontSize - CP_FONT_STEP_SIZE; 
        //extension->resized();
    } else if (buttonThatWasClicked == fontUp.get()) {
        chordProSetFontSize((chordProFontSize >= CP_MAX_FONT_SIZE) ? chordProFontSize : chordProFontSize + CP_FONT_STEP_SIZE);
        //chordProFontSize = (chordProFontSize >= CP_MAX_FONT_SIZE) ? chordProFontSize : chordProFontSize + CP_FONT_STEP_SIZE;
        //extension->resized();
    } else if (buttonThatWasClicked == fontMono.get()) {
        chordProMonospaceFont = !chordProMonospaceFont;
        extension->resized();
    } else if (buttonThatWasClicked == fontButton.get()) {
        fontButtonContainer.setVisible(!fontButtonContainer.isVisible());
        repaint();
    } else if (buttonThatWasClicked == lightDarkModeButton.get()) {
        toggleDarkMode();
        
    } else if (buttonThatWasClicked == columnsOneButton.get()) {
        setDisplayTwoPages(false);
    } else if (buttonThatWasClicked == columnsTwoButton.get()) {
        setDisplayTwoPages(true);
    } else if (buttonThatWasClicked == fitWidthButton.get()) {
        setFitToPageHeight(false);
    }  else if (buttonThatWasClicked == fitHeightButton.get()) {
        setFitToPageHeight(true);
    } else if (buttonThatWasClicked == createInvertedImage.get()) {
        extension->missingImageLabel->setText(CP_DARK_MODE_IMAGE_PROCESSING, dontSendNotification);
        createInvertedImage->setVisible(false);
        extension->repaint();
        imageTimer.startTimer(TIMER_IMAGE_CONVERT);
    } else if (buttonThatWasClicked == setlistButton.get()) {
        setlistContainer.setVisible(!setlistContainer.isVisible());
    } else if (buttonThatWasClicked->getProperties()["type"] == "setlistButton") {
        //log("Setlist Button" + buttonThatWasClicked->getName());
        lib->switchToSetlist(buttonThatWasClicked->getName().getIntValue());
    } else if (buttonThatWasClicked == prefButtons[0]) {
        toggleZeroBasedNumbering();
    } else if (buttonThatWasClicked == prefButtons[1]) {
        toggleLargeScrollArea();
    } else if (buttonThatWasClicked == prefButtons[2]) {
       toggleThickBorders();
       displayPreferences();
    } else if (buttonThatWasClicked == prefButtons[3]) {
        toggleVariationsInSetlistMode();
        displayPreferences();
    } else if (buttonThatWasClicked == prefButtons[4]) {
        toggleLeftMarginLabels();
        displayPreferences();
    } else if (buttonThatWasClicked == prefButtons[5]) {
        toggleSmallChordFont();
        displayPreferences();
    } else if (buttonThatWasClicked == closeButton.get()) {
        savePreferences();
        chordProReadFile(getButtonSelected());
    }
        //prefToggle->setState(Button::ButtonState::buttonNormal);
        /*
        bool status = extension->preferences->getProperty("ZeroBasedNumbers");
     extension->prefButtons[0]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("LargeScrollArea");
     extension->prefButtons[1]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("ThickBorders");
     extension->prefButtons[2]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("RackspaceVariationInSetlistMode");
     extension->prefButtons[3]->setToggleState(status, dontSendNotification);
   
        */
    
}

void ExtensionWindow::displayWindow(bool display) {
    if (!extension->prefsLoaded) {
        extension->pendingDisplayWindow = true;
        return;
    }
    if (extension->extensionWindow->isVisible() != display) {
        extension->extensionWindow->setVisible(display);
        lib->setWidgetValue(WIDGET_SELECTOR, (display == true ? 1.0 : 0.0));
    }
    if (display)
            extension->extensionWindow->toFront(true);
}

void ExtensionWindow::scrollWindow(double value) {
    Point<int> viewportPosition = extension->viewport.getViewPosition();
    Rectangle<int> viewportBounds = extension->viewport.getViewArea();
    Rectangle<int> containerBounds = extension->container.getBounds();
    int deltaH = containerBounds.getHeight() - viewportBounds.getHeight();
    deltaH = (deltaH < 0) ? 0 : deltaH;
    int newY = (int) (value * (double) deltaH);
    viewportPosition.setY(newY);
    extension->viewport.setViewPosition(viewportPosition);
}

void ExtensionWindow::initialize() {
    MessageManager::getInstance()->callAsync([]() {
        if (extension == nullptr) {
            extension = new ExtensionWindow();
            extension->extensionWindow->setTopLeftPosition(
                                            Rectangle<int>::fromString(DEFAULT_WINDOW_POSITION).getX(), 
                                            Rectangle<int>::fromString(DEFAULT_WINDOW_POSITION).getY()
                                        );
        }
        jassert(extension != nullptr);
        extension->extensionWindow->setVisible(false);
    });
}

void ExtensionWindow::finalize()
{
    savePreferences();
    delete lib;
    lib = nullptr;

    delete extension;
    extension = nullptr;
}

void ExtensionWindow::processPreferencesDefaults(StringPairArray prefs) {
    setZeroBasedNumbering(prefs.getValue("ZeroBasedNumbers", "") == "true" ? true : false);
    //extension->log(prefs.getValue("ZeroBasedNumbers", "") == "true" ? "true" : "false");
    //extension->preferences->setProperty("ImmediateSwitching", prefs.getValue("ImmediateSwitching", "") == "false" ? false : true);
    setLargeScrollArea(prefs.getValue("LargeScrollArea", "") == "true" ? true : false);
    removeColorKeywordFromName(prefs.getValue("RemoveColorKeywordFromName", "") == "true" ? true : false); 
    //StringArray positionSize = StringArray::fromTokens(prefs.getValue("PositionAndSize", DEFAULT_WINDOW_POSITION), ",", "");
    //setWindowPositionAndSize(positionSize[0].getIntValue(), positionSize[1].getIntValue(), positionSize[2].getIntValue(), positionSize[3].getIntValue());
    extension->preferences->setProperty("ThickBorders", prefs.getValue("ThickBorders", "") == "true" ? true : false);
    extension->preferences->setProperty("BorderColor", prefs.getValue("BorderColor", DEFAULT_BORDER_COLOR));
    setDarkMode(prefs.getValue("DarkMode", "") == "true" ? true : false);
    //displayRackspaceVariationInSetlistMode(prefs.getValue("DisplayRackspaceVariationInSetlistMode", "") == "true" ? true : false);
    //headerRackspaceColor = Colour::fromString(prefs.getValue("HeaderRackspaceColor", HEADER_RACKSPACE_COLOR));
    headerSongColor = Colour::fromString(prefs.getValue("HeaderSongColor", HEADER_SONG_COLOR));
    setDisplayVariationNames(prefs.getValue("DisplayVariationNames", "") == "true" ? true : false); 
    setFitToPageHeight(prefs.getValue("FitToPageHeight", "") == "true" ? true : false); 
    setDisplayTwoPages(prefs.getValue("DisplayTwoPages", "") == "true" ? true : false); 
    String fontSize = prefs.getValue("FontSize", "0");
    chordProSetFontSize(fontSize == "0" ? CP_DEFAULT_FONT_SIZE : fontSize.getFloatValue());
    chordProSetLeftMarginLabels(prefs.getValue("LeftMarginLabels", "") == "true" ? true : false); 
    chordProSetSmallChordFont(prefs.getValue("SmallChordFont", "") == "true" ? true : false); 

    extension->updatePreferencesWindow();
}

void ExtensionWindow::processPreferencesColors(StringPairArray prefs) {
    extension->buttonColors.addArray(prefs);
}

void ExtensionWindow::processPreferencesChordProColors(StringPairArray prefs) {
    extension->preferencesChordProColors->setProperty("LightModeChords", prefs.getValue("LightModeChords", CP_LIGHT_CHORD_COLOR));
    extension->preferencesChordProColors->setProperty("LightModeLyrics", prefs.getValue("LightModeLyrics", CP_LIGHT_LYRIC_COLOR));
    extension->preferencesChordProColors->setProperty("LightModeBackground", prefs.getValue("LightModeBackground", CP_LIGHT_BACKGROUND_COLOR));
    extension->preferencesChordProColors->setProperty("DarkModeChords", prefs.getValue("DarkModeChords", CP_DARK_CHORD_COLOR));
    extension->preferencesChordProColors->setProperty("DarkModeLyrics", prefs.getValue("DarkModeLyrics", CP_DARK_LYRIC_COLOR));
    extension->preferencesChordProColors->setProperty("DarkModeBackground", prefs.getValue("DarkModeBackground", CP_DARK_BACKGROUND_COLOR));

    //extension->chordProColors.addArray(prefs);
    extension->chordProSetColors();
    extension->updatePreferencesWindow();

}

void ExtensionWindow::processPreferencesWindowState(StringPairArray prefs) {
    StringArray positionSize = StringArray::fromTokens(prefs.getValue("PositionAndSize", DEFAULT_WINDOW_POSITION), ",", "");
    bool extendedWindow = prefs.getValue("ExpandedWindow", "") == "false" ? false : true;
    int divider = prefs.getValue("WindowDivider", "250").getIntValue();
    //std::string test = extendedWindow ? "Expanded" : "Not Expanded ";
    extension->extensionWindow->setTopLeftPosition(positionSize[0].getIntValue(), positionSize[1].getIntValue());
    extension->extensionWindow->setSize(positionSize[2].getIntValue(), positionSize[3].getIntValue());
    if (extendedWindow) {
        extension->displayRightPanel = true;
        extension->draggableResizer.setBounds(extension->draggableResizer.getBounds().withX(divider));
        displayExpandedWindow(true);
    }
    extension->resized();
    extension->prefsLoaded = true;
    if (extension->pendingDisplayWindow) displayWindow(true);
}

void ExtensionWindow::updatePreferencesWindow() {
     bool status = extension->preferences->getProperty("ZeroBasedNumbers");
     extension->prefButtons[0]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("LargeScrollArea");
     extension->prefButtons[1]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("ThickBorders");
     extension->prefButtons[2]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("DisplayVariationNames");
     extension->prefButtons[3]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("LeftMarginLabels");
     extension->prefButtons[4]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("SmallChordFont");
     extension->prefButtons[5]->setToggleState(status, dontSendNotification);

    String colorText = extension->preferencesChordProColors->getProperty("LightModeChords").toString();;
    Colour color = Colour::fromString(colorText);
    Colour textColor = color.getBrightness() > 0.6 ? Colours::black : Colours::white;
   
    extension->prefColorLabels[0]->setColour(Label::backgroundColourId, color);
    extension->prefColorLabels[0]->setText(colorText, dontSendNotification);
    extension->prefColorLabels[0]->setColour(Label::textColourId, textColor);
    extension->prefColorLabels[0]->setColour(Label::textWhenEditingColourId, textColor);

    colorText = extension->preferencesChordProColors->getProperty("LightModeLyrics").toString();
    color = Colour::fromString(colorText);
    textColor = color.getBrightness() > 0.6 ? Colours::black : Colours::white;

    extension->prefColorLabels[1]->setColour(Label::backgroundColourId, color);
    extension->prefColorLabels[1]->setText(colorText, dontSendNotification);
    extension->prefColorLabels[1]->setColour(Label::textColourId, textColor);
    extension->prefColorLabels[1]->setColour(Label::textWhenEditingColourId, textColor);


    colorText = extension->preferencesChordProColors->getProperty("DarkModeChords").toString();
    color = Colour::fromString(colorText);
    textColor = color.getBrightness() > 0.6 ? Colours::black : Colours::white;

    extension->prefColorLabels[2]->setColour(Label::backgroundColourId, color);
    extension->prefColorLabels[2]->setText(colorText, dontSendNotification);
    extension->prefColorLabels[2]->setColour(Label::textColourId, textColor);
    extension->prefColorLabels[2]->setColour(Label::textWhenEditingColourId, textColor);
    
    colorText = extension->preferencesChordProColors->getProperty("DarkModeLyrics").toString();
    color = Colour::fromString(colorText);
    textColor = color.getBrightness() > 0.6 ? Colours::black : Colours::white;
    
    extension->prefColorLabels[3]->setColour(Label::backgroundColourId, color);
    extension->prefColorLabels[3]->setText(colorText, dontSendNotification);
    extension->prefColorLabels[3]->setColour(Label::textColourId, textColor);
    extension->prefColorLabels[3]->setColour(Label::textWhenEditingColourId, textColor);
}

void ExtensionWindow::updatePreferencesColors() {
    String colorText = extension->prefColorLabels[0]->getText();
    Colour color = Colour::fromString(colorText);
    Colour textColor = color.getBrightness() > 0.6 ? Colours::black : Colours::white;
    
    extension->preferencesChordProColors->setProperty("LightModeChords", colorText);
    extension->prefColorLabels[0]->setColour(Label::backgroundColourId, color);
    extension->prefColorLabels[0]->setColour(Label::textColourId, textColor);
    extension->prefColorLabels[0]->setColour(Label::textWhenEditingColourId, textColor);

    colorText = extension->prefColorLabels[1]->getText();
    color = Colour::fromString(colorText);
    textColor = color.getBrightness() > 0.6 ? Colours::black : Colours::white;
    
    extension->preferencesChordProColors->setProperty("LightModeLyrics", colorText);
    extension->prefColorLabels[1]->setColour(Label::backgroundColourId, color);
    extension->prefColorLabels[1]->setColour(Label::textColourId, textColor);
    extension->prefColorLabels[1]->setColour(Label::textWhenEditingColourId, textColor);

    colorText = extension->prefColorLabels[2]->getText();
    color = Colour::fromString(colorText);
    textColor = color.getBrightness() > 0.6 ? Colours::black : Colours::white;
    
    extension->preferencesChordProColors->setProperty("DarkModeChords", colorText);
    extension->prefColorLabels[2]->setColour(Label::backgroundColourId, color);
    extension->prefColorLabels[2]->setColour(Label::textColourId, textColor);
    extension->prefColorLabels[2]->setColour(Label::textWhenEditingColourId, textColor);

    colorText = extension->prefColorLabels[3]->getText();
    color = Colour::fromString(colorText);
    textColor = color.getBrightness() > 0.6 ? Colours::black : Colours::white;
    
    extension->preferencesChordProColors->setProperty("DarkModeLyrics", colorText);
    extension->prefColorLabels[3]->setColour(Label::backgroundColourId, color);
    extension->prefColorLabels[3]->setColour(Label::textColourId, textColor);
    extension->prefColorLabels[3]->setColour(Label::textWhenEditingColourId, textColor);
}

void ExtensionWindow::removeColorKeywordFromName(bool remove) {
    extension->preferences->setProperty("RemoveColorKeywordFromName", remove); 
}

void ExtensionWindow::updateClock(const String& formattedTime) {
    extension->clock->setText(formattedTime, dontSendNotification);
}

void ExtensionWindow::chordProScrollWindow(double value) {
    Point<int> viewportPosition = extension->viewportRight.getViewPosition();
    Rectangle<int> viewportBounds = extension->viewportRight.getViewArea();
    Rectangle<int> containerBounds = extension->chordProContainer.getBounds();
    int deltaH = containerBounds.getHeight() - viewportBounds.getHeight();
    deltaH = (deltaH < 0) ? 0 : deltaH;
    int newY = (int) (value * (double) deltaH);
    viewportPosition.setY(newY);
    extension->viewportRight.setViewPosition(viewportPosition);
}

void ExtensionWindow::chordProUp() {
    Rectangle<int> viewArea = extension->viewportRight.getViewArea();
    extension->viewportRight.setViewPosition(0, viewArea.getY() - viewArea.getHeight());

}

void ExtensionWindow::chordProDown() {
    Rectangle<int> viewArea = extension->viewportRight.getViewArea();
    extension->viewportRight.setViewPosition(0, viewArea.getY() + viewArea.getHeight());

}

void ExtensionWindow::chordProScrollToSongPart(std::string songPartName) {
    for (int i = 0; i < extension->chordProLines.size(); ++i) { 
        if (extension->chordProLines[i]->getProperties()["type"] == "gp_songpartname") {
            if (extension->chordProLines[i]->getText().toStdString() == songPartName) {
                Rectangle<int> buttonBounds = extension->chordProLines[i]->getBounds();
                extension->viewportRight.setViewPosition(0, buttonBounds.getY());
            }
        }
    }
}

void ExtensionWindow::chordProProcessText(std::string text) {
    StringArray lines = StringArray::fromLines(text);
    String line;
    int firstLineWithContent = false;
    int imageCount = 0;
    StringArray directiveParts;
    String directiveName;
    String directiveValue;
    bool tabLine = false;
    //bool tabLineFirst = false;
    String tabLabel;
    bool gridLine = false;
    //bool gridLineFirst = false;
    String gridLabel;
    int gridBarCharacterLength = 0;
    bool chorusLine = false;
    //bool chorusLineFirst = false;
    String chorusLabel;
    String sectionLabel;
    bool firstSectionLine = false;
    extension->chordProReset();
    bool chordProNonImage = false;

    // Extend size if needed
    if (lines.size() > extension->chordProLines.size() - 1) {
        int priorLines = extension->chordProLines.size();
        int newLines = lines.size() - priorLines + 1;
        for (int i = 0; i < newLines; ++i) { 
            std::string number = std::to_string(priorLines + i);
            auto label = new Label(number, number); 
            extension->chordProLines.add(label);
            extension->chordProLines[i]->setEditable (false, false, false);
            extension->chordProLines[i]->setLookAndFeel(extension->chordProLnF);
            extension->chordProLines[i]->setVisible(true);
        }
    }

    for (int i = 0; i < lines.size() + 1; ++i) { 
        line = lines[i].toStdString();
        if (line.trim() != "") firstLineWithContent = true;
        if (firstLineWithContent) {
            extension->chordProLines[i]->setVisible(true);
            extension->chordProLines[i]->getProperties().set("section", ""); 
            extension->chordProLines[i]->getProperties().set("sectionLabel", ""); 
            if (line.contains("{")) { // Directive
                extension->chordProLines[i]->getProperties().set("type", "directive"); 
                line = line.removeCharacters("{}");
                if (line.contains(":")) {
                    directiveParts = StringArray::fromTokens(line,":",""); // Split directive
                    directiveName = directiveParts[0].removeCharacters(" ").toLowerCase();
                    directiveValue = directiveParts[1].trim();
                    if (directiveName == "title" || directiveName == "t") {
                        extension->chordProLines[i]->setLookAndFeel(extension->chordProTitleLnF);
                        extension->chordProLines[i]->getProperties().set("type", "title"); 
                    } else if (directiveName == "subtitle" || directiveName == "st") {
                        extension->chordProLines[i]->setLookAndFeel(extension->chordProSubTitleLnF);
                        extension->chordProLines[i]->getProperties().set("type", "subtitle"); 
                    } else if (directiveName == "comment" || directiveName == "c") {
                        extension->chordProLines[i]->setLookAndFeel(extension->chordProCommentLnF);
                        extension->chordProLines[i]->getProperties().set("type", "comment"); 
                    } else if (directiveName == "songpartname") {
                        extension->chordProLines[i]->setVisible(false);
                        extension->chordProLines[i]->getProperties().set("type", "gp_songpartname"); 
                    } else if (directiveName.contains("start_of") || directiveName.contains("end_of") || (directiveName.length() == 3 && (directiveName.contains("so") || directiveName.contains("eo")))) {
                        extension->chordProLines[i]->setLookAndFeel(extension->chordProLabelLnF);
                        extension->chordProLines[i]->getProperties().set("type", "label"); 
                        extension->chordProLines[i]->setVisible(!chordProLeftLabels);
                        
                    } else if (directiveName == "image") {
                            extension->chordProLines[i]->getProperties().set("type", "image");                             
                            #if JUCE_WINDOWS
                                if (directiveParts.size() == 3) { // File path had a drive letter e.g. C:
                                    directiveValue = directiveParts[1].trim() + ":" + directiveParts[2].trim();
                                }  
                            #endif
                            String path = directiveValue.removeCharacters("\"");
                            File file = File(path);
                            File fileDarkMode;
                            String pathDarkMode;
                            Image image;
                            bool createDarkMode = false;
                            if (extension->chordProDarkMode) {
                                pathDarkMode = file.getFileNameWithoutExtension() + CP_DARK_MODE_FILE_SUFFIX + file.getFileExtension();
                                fileDarkMode = file.getParentDirectory().getChildFile(pathDarkMode);
                                image = ImageFileFormat::loadFrom(fileDarkMode);
                                if (!image.isValid()) {
                                    createDarkMode = true;
                                    image = ImageFileFormat::loadFrom(file);
                                }
                            } else {
                                image = ImageFileFormat::loadFrom(file);
                            }
                            if (image.isValid()) {
                                extension->chordProImagesCheckAndAdd(imageCount);
                                extension->chordProImages[imageCount]->setImage(image);
                                extension->chordProImages[imageCount]->setImagePlacement (RectanglePlacement (RectanglePlacement::fillDestination | RectanglePlacement::xLeft | RectanglePlacement::yTop));
                                extension->chordProImages[imageCount]->setVisible(true);
                                extension->chordProImages[imageCount]->getProperties().set("path", file.getFullPathName()); 
                                extension->chordProLines[i]->getProperties().set("imageIndex", imageCount);
                                imageCount++;
                            } else {
                                extension->log("Image Not Found: " + file.getFullPathName());
                                createDarkMode = false;
                            }
                            directiveValue = "";
                            extension->missingImageContainer.setVisible(createDarkMode && extension->displayRightPanel);
                    } else {
                        extension->chordProLines[i]->setLookAndFeel(extension->chordProSubTitleLnF);
                        extension->chordProLines[i]->getProperties().set("type", "other"); 
                        directiveValue = directiveName.substring(0,1).toUpperCase() + directiveName.substring(1,directiveName.length()) + ": " + directiveValue;
                    } 
                } else {
                    extension->chordProLines[i]->setVisible(false);
                    directiveName = line.removeCharacters(" ");
                    directiveValue = "";
                }
                if (directiveName == "start_of_tab" || directiveName == "sot") {
                    tabLine = true;
                    //tabLineFirst = true;
                    firstSectionLine = true;
                    tabLabel = directiveValue == "" ? "Tab" : directiveValue;
                    sectionLabel = directiveValue == "" ? "Tab" : directiveValue;

                } else if (directiveName == "end_of_tab" || directiveName == "eot") {
                    tabLine = false;
                    //tabLineFirst = false;
                } else if (directiveName.startsWith("start_of_grid") || directiveName.startsWith("sog")) {
                    gridLine = true;
                    //gridLineFirst = true;
                    firstSectionLine = true;
                    gridLabel = directiveValue == "" ? "Grid" : directiveValue;
                    sectionLabel = directiveValue == "" ? "Grid" : directiveValue;

                } else if (directiveName == "end_of_grid" || directiveName == "eog") {
                    gridLine = false;
                    //gridLineFirst = false;
                } else if (directiveName == "start_of_chorus" || directiveName == "soc") {
                    chorusLine = true;
                    //chorusLineFirst = true;
                    firstSectionLine = true;
                    chorusLabel = directiveValue == "" ? "Chorus" : directiveValue;
                    sectionLabel = directiveValue == "" ? "Chorus" : directiveValue;

                } else if (directiveName == "end_of_chorus" || directiveName == "eoc") {
                    chorusLine = false;
                    //chorusLineFirst = false;
                } else if (directiveName == "start_of_verse" || directiveName == "sov") {
                    firstSectionLine = true;
                    sectionLabel = directiveValue == "" ? "Verse" : directiveValue;

                } else if (directiveName == "end_of_verse" || directiveName == "eov") {
                    //chorusLine = false;
                    //chorusLineFirst = false;
                } else {
                    firstSectionLine = false;
                }
                extension->chordProLines[i]->setText(directiveValue, dontSendNotification);
            } else {
                if (tabLine) {
                    extension->chordProLines[i]->setLookAndFeel(extension->chordProTabLnF);
                    extension->chordProLines[i]->getProperties().set("type", "tab");
                    //if (tabLineFirst) extension->chordProLines[i]->getProperties().set("sectionLabel", tabLabel);
                    //tabLineFirst = false;
                } else if (gridLine) {
                    extension->chordProLines[i]->setLookAndFeel(extension->chordProGridLnF);
                    extension->chordProLines[i]->getProperties().set("type", "grid");
                    //if (gridLineFirst) extension->chordProLines[i]->getProperties().set("sectionLabel", gridLabel);
                    //gridLineFirst = false;

                    // Store any grid comment and then remove from grid line
                    String postComment = line.substring(line.lastIndexOf("|") + 1, line.length()).trim(); 
                    extension->chordProLines[i]->getProperties().set("gridComment", postComment);
                    line = line.substring(0, line.lastIndexOf("|") + 1); // Exclude comments after the bar

                    // Calculate maximum character length of each bar
                    String temp = line.replace(" ","");
                    temp = temp.replace("||","|").replace("|:","|").replace(":|","|"); // Convert special characters to single character
                    
                    StringArray parts = StringArray::fromTokens(temp,"|","");
                    parts.removeEmptyStrings(true);
                    for (int j = 0; j < parts.size(); ++j) {
                        int length = parts[j].length();
                        if (length > gridBarCharacterLength) {
                            gridBarCharacterLength = length;
                            //extension->log("Grid bar length: " + std::to_string(gridBarCharacterLength));

                        }
                    }
                    extension->chordProLines[i]->getProperties().set("gridBars", parts.size());
                    //extension->log("Grid bars: " + std::to_string(parts.size()));
                } else if (chorusLine) {
                    extension->chordProLines[i]->getProperties().set("section", "chorus"); 
                    //if (chorusLineFirst) extension->chordProLines[i]->getProperties().set("sectionLabel", chorusLabel);
                    //chorusLineFirst = false;
                }
                if (firstSectionLine) {
                    extension->chordProLines[i]->getProperties().set("sectionLabel", sectionLabel);
                    sectionLabel = "";
                    firstSectionLine = false;
                }
                if (line.contains("[")) {
                    String excludeChords = std::regex_replace(line.toStdString(), std::regex("\\[(.*?)\\]"), "");
                    if (excludeChords.trim() == "") {
                        extension->chordProLines[i]->getProperties().set("type", "chordOnly"); 
                        StringArray words = StringArray::fromTokens(lines[i],false);
                        for (int j = 0; j < words.size(); ++j) { 
                             String word = words[j].toStdString();
                            if  (word != "") {
                                if (!word.startsWith("[")) {
                                    word = "[" + word;
                                }
                                if (!word.endsWith("]")) {
                                    word = word + "]";
                                }  
                                words.set(j, word);
                            }
                        }
                        line = words.joinIntoString(" ", 0, -1);
                    } else {
                        extension->chordProLines[i]->getProperties().set("type", "chordAndLyrics"); 
                    }
                } else if (line.trim() != "" && !gridLine && !tabLine){
                    extension->chordProLines[i]->getProperties().set("type", "lyricOnly"); 
                }
                extension->chordProLines[i]->setText(line.trim(), dontSendNotification);
            }
        }
        if (!(directiveName == "image" || directiveName == "songpartname" || (extension->chordProLines[i]->getProperties()["type"] == ""))) {
            chordProNonImage = true;
        }
    }
    if (!chordProNonImage) extension->chordProImagesOnly = true;
    if (gridBarCharacterLength > 0) {
        //extension->log("Grid Bar: " + std::to_string(gridBarCharacterLength));
        for (int i = 0; i < extension->chordProLines.size(); ++i) { 
            //extension->log("Line " + extension->chordProLines[i]->getProperties()["type"].toString());
            if (extension->chordProLines[i]->getProperties()["type"] == "grid") {
                //extension->log("Grid line " + std::to_string(i));
                extension->chordProLines[i]->getProperties().set("gridBarLength", gridBarCharacterLength); 
                //extension->log("Grid bar: " + std::to_string(gridBarCharacterLength));
            }
        }
    }
    extension->chordProDisplayGUI(true);
}

void ExtensionWindow::chordProReadFile(int index) {
    if (lib == nullptr) return;
    std::string chordProFileText;
    std::string chordProFile = lib->getChordProFilenameForSong(index);
    //extension->log(std::to_string(index) + ": " + chordProFile);
    extension->chordProForCurrentSong = (chordProFile == "") ? false : true;
    if (extension->chordProForCurrentSong) {
        File chordProFullPath = File(chordProFile);
        if (chordProFullPath.existsAsFile()) {
            gigperformer::sdk::GPUtils::loadTextFile(chordProFullPath.getFullPathName().toStdString(), chordProFileText);
            chordProProcessText(chordProFileText);   
            extension->noChordProLabel->setVisible(false);
        } else {
            extension->chordProForCurrentSong = false;
        }
    }
    if (!extension->chordProForCurrentSong) {
        extension->chordProDisplayGUI(false);
        extension->chordProReset();
    }

}

void ExtensionWindow::chordProRefresh() {
    Point<int> viewRightPos = viewportRight.getViewPosition();
    int songIndex = lib->inSetlistMode() ? lib->getCurrentSongIndex() : extension->getButtonSelected();
    extension->chordProReadFile(songIndex);
    extension->chordProDisplayGUI(true);
    viewportRight.setViewPosition(viewRightPos);
}

void ExtensionWindow::chordProReset() {
    for (int i = 0; i < extension->chordProLines.size(); ++i) { 
        extension->chordProLines[i]->setLookAndFeel(extension->chordProLnF);
        extension->chordProLines[i]->setText("", dontSendNotification);
        extension->chordProLines[i]->setVisible(false);
        extension->chordProLines[i]->getProperties().set("type", ""); 
    }
    for (int i = 0; i < extension->chordProImages.size(); ++i) { 
        extension->chordProImages[i]->setVisible(false);
        extension->chordProImages[i]->getProperties().set("path", ""); 
    }
    missingImageContainer.setVisible(false);
    extension->chordProImagesOnly = false;
    extension->noChordProLabel->setVisible(true);
}

void ExtensionWindow::chordProDisplayGUI(bool display) { 
    fontButton->setVisible(display && displayRightPanel && !chordProImagesOnly);
    lightDarkModeButton->setVisible(display && displayRightPanel);
    columnsOneButton->setVisible(display && displayRightPanel && chordProImagesOnly && chordProTwoColumns);
    columnsTwoButton->setVisible(display && displayRightPanel && chordProImagesOnly && !chordProTwoColumns);
    fitWidthButton->setVisible(display && displayRightPanel && chordProImagesOnly && fitHeight);
    fitHeightButton->setVisible(display && displayRightPanel && chordProImagesOnly && !fitHeight);
    preferencesContainer.setVisible(false);
    fontButtonContainer.setVisible(false);
    if (display) { 
        extension->viewportRight.setViewedComponent(&extension->chordProContainer, false);
        extension->chordProContainer.setVisible(true);
        extension->chordProSetColors();
    } else {
        missingImageContainer.setVisible(false);
        if (!chordProForCurrentSong) {
            extension->viewportRight.setViewedComponent(&extension->containerRight, false);
            extension->containerRight.setVisible(true);
            viewPortBackground = Colour::fromString(BACKGROUND_COLOR);
        }
    }
    extension->viewportRight.setViewPosition(0,0);
    extension->resized();
}

void ExtensionWindow::chordProSetColors() { 
    if (chordProDarkMode) {
        chordProLyricColor = Colour::fromString(extension->preferencesChordProColors->getProperty("DarkModeLyrics").toString());
        chordProChordColor = Colour::fromString(extension->preferencesChordProColors->getProperty("DarkModeChords").toString());
        if (chordProImagesOnly) {
            viewPortBackground = Colour::fromString(CP_DARK_IMAGES_BACKGROUND_COLOR);
        } else {
            viewPortBackground = Colour::fromString(extension->preferencesChordProColors->getProperty("DarkModeBackground").toString());
        }
    } else {
        chordProLyricColor = Colour::fromString(extension->preferencesChordProColors->getProperty("LightModeLyrics").toString());
        chordProChordColor = Colour::fromString(extension->preferencesChordProColors->getProperty("LightModeChords").toString());
        if (chordProImagesOnly) {
            viewPortBackground = Colour::fromString(CP_LIGHT_IMAGES_BACKGROUND_COLOR);
        } else {
            viewPortBackground = Colour::fromString(extension->preferencesChordProColors->getProperty("LightModeBackground").toString());
        }
    }
}

void ExtensionWindow::chordProSetFontSize(float newSize) {
    extension->preferences->setProperty("FontSize", newSize);    
    chordProFontSize = newSize;
    extension->resized();
}

void ExtensionWindow::chordProSetSmallChordFont(bool isSmall) {
    extension->preferences->setProperty("SmallChordFont", isSmall);
    chordProSmallChordFont = isSmall;
}

void ExtensionWindow::chordProImagesCheckAndAdd(int index) {
    int existingCount = extension->chordProImages.size();
    int newCount = index + 1;
    if (newCount > existingCount) {
        int newImages = newCount - existingCount;
        for (auto i = 0; i < newImages; ++i) {     
            auto imageComponent = new ImageComponent(std::to_string(existingCount+i));
            chordProImages.add(imageComponent);
            chordProContainer.addAndMakeVisible(chordProImages[existingCount+i]);
        }
    }
} 

int ExtensionWindow::chordProGetVisibleImageCount() {
    int visible = 0;
    for (auto i = 0; i < extension->chordProImages.size(); ++i) {
        if (extension->chordProImages[i]->isVisible()) {
            ++visible;
        }
    }
    return visible;
}

void ExtensionWindow::chordProCreateInvertedImages() {
    //MessageManager::getInstance()->callAsync([]() {
        for (int i = 0; i < extension->chordProImages.size(); ++i) { 
            if (extension->chordProImages[i]->isVisible()) {
                String path = extension->chordProImages[i]->getProperties()["path"];
                if (!path.contains(CP_DARK_MODE_FILE_SUFFIX)) {
                    File file = File(path);
                    Image image = ImageFileFormat::loadFrom(file);
                    if (image.isValid()) {
                        for(int y = 0; y < image.getHeight(); y++) {
                            for(int x = 0; x < image.getWidth(); x++) {
                                Colour col = image.getPixelAt(x, y);
                                uint8 r = 255 - col.getRed();
                                uint8 g = 255 - col.getGreen();
                                uint8 b = 255 - col.getBlue();
                                image.setPixelAt(x, y, Colour::fromRGB(r, g, b));
                            }   
                        }
                        image = image.getClippedImage(image.getBounds().withTrimmedTop(1));
                        String pathDarkMode = file.getFileNameWithoutExtension() + CP_DARK_MODE_FILE_SUFFIX + file.getFileExtension();
                        File fileDarkMode = file.getParentDirectory().getChildFile(pathDarkMode);
                        auto destStream = fileDarkMode.createOutputStream();
                        PNGImageFormat pngimage;
                        pngimage.writeImageToStream(image, *destStream);
                        extension->chordProImages[i]->setImage(image);
                        extension->chordProImages[i]->getProperties().set("path", fileDarkMode.getFullPathName()); 
                    }
                }
            }
        }
        extension->missingImageContainer.setVisible(false);
        extension->missingImageLabel->setText(CP_NO_DARK_MODE_IMAGES, dontSendNotification);
        extension->createInvertedImage->setVisible(true);
   // });
}

void ExtensionWindow::log(String text) {
    if (lib == nullptr) return;
    lib->consoleLog(LOG_PREFIX + text.toStdString());
}

void ExtensionWindow::setSongLabel() {
    //if (!lib->inSetlistMode()) return;
    /*
    int divider = extension->draggableResizer.getX();
    if (divider <= 10) {
        std::string songName = lib->getSongName(lib->getCurrentSongIndex());
        if (extension->header->getText().toStdString() != songName) {
            extension->header->setText(songName, dontSendNotification);
            extension->resized();
        }
    } else {
        if (extension->header->getText().toStdString() != SONG_TITLE) {
             extension->header->setText(SONG_TITLE, dontSendNotification);
             extension->resized();
        }
    }
    */
    int songIndex = lib->inSetlistMode() ? lib->getCurrentSongIndex() : extension->getButtonSelected();
    std::string songName = lib->getSongName(songIndex);
    extension->header->setText(songName, dontSendNotification);
    extension->resized();
}

void ExtensionWindow::displayFontContainer(bool display) {
    extension->fontButtonContainer.setVisible(display);
}

void ExtensionWindow::displaySetlistContainer(bool display) {
    extension->setlistContainer.setVisible(display);
}

void ExtensionWindow::displayPreferences() {
    extension->chordProContainer.setVisible(false);
    extension->containerRight.setVisible(false);
    extension->preferencesContainer.setVisible(true);
    viewPortBackground = Colour::fromString(BACKGROUND_COLOR);

    //String test = getDefaults();
    /*
    if (extension->chordProDarkMode) {
        viewPortBackground = Colour::fromString(CP_DARK_BACKGROUND_COLOR);
    } else {
        viewPortBackground = Colour::fromString(CP_LIGHT_BACKGROUND_COLOR);
    }
    */
    //extension->chordProSetColors();
    //extension->viewportRight.setViewedComponent(&extension->preferencesContainer);
}

void ExtensionWindow::setSongPanelPosition(bool display) {
    auto bounds = extension->draggableResizer.getBounds();
    int newX = display ? 250 : 0;
    extension->draggableResizer.setBounds(bounds.withX(newX));
    extension->container.setBounds(extension->container.getBounds().withWidth(newX));
    extension->sidePanelCloseButton->setVisible(display);
    extension->sidePanelOpenButton->setVisible(!display);
    extension->resized();
}

void ExtensionWindow::checkSongListPosition() {
    String headerLabel = extension->header->getText();
    Font headerLabelFont = extension->header->getFont();
    int headerLabelWidth = headerLabelFont.getStringWidth(headerLabel);
    bool display = extension->draggableResizer.getX() > 0;
    extension->sidePanelCloseButton->setVisible(display && extension->sidePanelCloseButton->getX() > headerLabelWidth + 20);
    extension->sidePanelOpenButton->setVisible(!display && extension->sidePanelOpenButton->getX() > headerLabelWidth + 20);
    extension->displaySongPanel = display;
    extension->resized();
}

String ExtensionWindow::getDefaults() {
    //String expandedWindow = extension->displayRightPanel ? "ExpandedWindow = true" : "ExpandedWindow = false";
    //String divider = "WindowDivider = " + String(extension->draggableResizer.getX());
    //return positionSize + "\n" + expandedWindow + "\n" + divider;
    String defaults;
    NamedValueSet prefs = extension->preferences->getProperties();
    for (int i = 0; i < prefs.size(); ++i) { 
    //for (const auto& pref : prefs) {
        String value = prefs.getValueAt(i).toString();
        if (value == "0") { value = "false";}
        else if (value == "1") { value = "true";}
        //extension->log(prefs.getName(i).toString() + " = " + value);
        defaults += (prefs.getName(i).toString() + " = " + value + "\n");

    }
    return defaults;
}

String ExtensionWindow::getChordProColors() {
    String text;
    NamedValueSet prefs = extension->preferencesChordProColors->getProperties();
    for (int i = 0; i < prefs.size(); ++i) { 
        String value = prefs.getValueAt(i).toString();
        text += (prefs.getName(i).toString() + " = " + value + "\n");
    }
    return text;
}

String ExtensionWindow::getWindowState() {
    Rectangle<int> window = getWindowPositionAndSize();
    String positionSize = "PositionAndSize = " + String(window.getX()) + "," + String(window.getY()) + "," + String(window.getWidth()) + "," + String(window.getHeight());
    //String expandedWindow = extension->displayRightPanel ? "ExpandedWindow = true" : "ExpandedWindow = false";
    String divider = "WindowDivider = " + String(extension->draggableResizer.getX());
    return positionSize + "\n" + divider;
}

void ExtensionWindow::saveWindowState() {
    if (!extension->prefsLoaded) return;
    String windowState = getWindowState();
    String path = extensionPath + PREF_FILENAME;
    File prefs = File(path);
    prefs.create();
    String content = prefs.loadFileAsString();
    int position = content.indexOf("[WindowLastSavedState]");
    if (position > 0) {
        content = content.substring(0, position);
    }
    content = content.trimEnd() + "\n\n[WindowLastSavedState]\n" + windowState;
    prefs.replaceWithText(content);
}

void ExtensionWindow::savePreferences() {
    if (!extension->prefsLoaded) return;

    String path = extensionPath + PREF_FILENAME;
    File prefs = File(path);
    prefs.create();
    String content;// = prefs.loadFileAsString();

    // Defaults
    String defaults = getDefaults();

    // ChordPro Colors
    String chordProColors = getChordProColors();

    // Window size/position
    String windowState = getWindowState();
    /*
    int position = content.indexOf("[WindowLastSavedState]");
    if (position > 0) {
        content = content.substring(0, position);
    }
    content = content.trimEnd() + "\n\n[WindowLastSavedState]\n" + windowState;
    */
    content = "[Defaults] \n" + defaults + "\n[LyricChordColors]\n" + chordProColors + "\n[WindowLastSavedState]\n" + windowState;
    prefs.replaceWithText(content);
}

void ExtensionWindow::restartWindowTimer() {
    extension->windowTimer.stopTimer();
    extension->windowTimer.startTimer(TIMER_WINDOW_STATE);
}

void MyDocumentWindow::closeButtonPressed() { 
    //ExtensionWindow::saveWindowState();
    ExtensionWindow::savePreferences();
    ExtensionWindow::displayWindow(false);
}

void MyDocumentWindow::moved() { 
    if (ExtensionWindow::extension != nullptr)
        ExtensionWindow::restartWindowTimer();
}

void ChordProContainer::mouseDown (const MouseEvent& e)
{
    ExtensionWindow::displayFontContainer(false);
    ExtensionWindow::displaySetlistContainer(false);
}

void ClockTimer::timerCallback() {
    ExtensionWindow::updateClock(Time::getCurrentTime().toString(false, true, false, true));
}

void RefreshTimer::timerCallback() {
    //if (!lib->inSetlistMode() && lockToSetlistMode) return;
    //ExtensionWindow::compareButtonNames(lib->inSetlistMode() ? lib->getSongNames() : lib->getRackspaceNames());
    if (lib == nullptr) return;
    ExtensionWindow::compareButtonNames(lib->getSongNames());
    ExtensionWindow::compareSubButtonNames(ExtensionWindow::getSubButtonNamesByIndex(ExtensionWindow::getButtonSelected()));
    ExtensionWindow::checkSongListPosition();
    //ExtensionWindow::setSongLabel();
}

void CreateImageTimer::timerCallback() {
    ExtensionWindow::chordProCreateInvertedImages();
    this->stopTimer();
}

void WindowChangeTimer::timerCallback() {
    //ExtensionWindow::saveWindowState();
    ExtensionWindow::savePreferences();
    this->stopTimer();
}