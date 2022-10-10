// Rackspace and song selector extension for Gig Performer by @rank13

#include <juce_events/juce_events.h>
#include "ExtensionWindow.h"
#include "LibMain.h"
#include "IconPaths.h"

ExtensionWindow* ExtensionWindow::extension = nullptr;
LibMain* lib = new LibMain(nullptr);   
Colour chordProLyricColor = Colour::fromString(CP_DARK_LYRIC_COLOR);  
Colour chordProChordColor = Colour::fromString(CP_DARK_CHORD_COLOR);
bool chordProMonospaceFont = false;
bool chordProDarkMode = true;
Colour viewPortBackground = Colour::fromString(BACKGROUND_COLOR);
float chordProFontSize = CP_DEFAULT_FONT_SIZE;

ExtensionWindow::ExtensionWindow ()
{
    LookAndFeel::setDefaultLookAndFeel(buttonsLnF);
    clockTimer.startTimer (5000);
    refreshTimer.startTimer(1000);
    bool zeroBasedNumbering = false;

    preferences.reset (new DynamicObject);
    preferences->setProperty("ImmediateSwitching", true);
    preferences->setProperty("LargeScrollArea", false);
    preferences->setProperty("ZeroBasedNumbers", false);
    preferences->setProperty("RemoveColorKeywordFromName", false);
    preferences->setProperty("ThickBorders", false);

    header.reset (new Label ("header", ""));
    addAndMakeVisible (header.get());
    header->setEditable (false, false, false);
    header->setBounds (0, 0, getWidth(), 50);
    header->setLookAndFeel(headerRackspacesLnF);
    
    label.reset (new Label ("title", ""));
    addAndMakeVisible (label.get());
    label->setFont (Font (25.00f, Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setBounds (15, 10, 336, 25);

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

    Path p;
    p.loadPathFromData (sidePanelOpenPathData, sizeof (sidePanelOpenPathData));
    sidePanelOpenButton.reset (new ShapeButton ( "sidePanelOpenButton", Colours::white, Colours::lightgrey, Colours::white ));
    sidePanelOpenButton->setShape (p, true, true, false);
    sidePanelOpenButton->setClickingTogglesState(true);
    sidePanelOpenButton->setTooltip("Expand window to display large song/rackspace titles");
    sidePanelOpenButton->addListener (this);
    addAndMakeVisible (sidePanelOpenButton.get());

    Path p2;
    p2.loadPathFromData (sidePanelClosePathData, sizeof (sidePanelClosePathData));
    sidePanelCloseButton.reset (new ShapeButton ( "sidePanelCloseButton", Colours::white, Colours::lightgrey, Colours::white ));
    sidePanelCloseButton->setShape (p2, true, true, false);
    sidePanelCloseButton->setClickingTogglesState(true);
    sidePanelCloseButton->setTooltip("Close expanded window");
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
    btnModeSwitch->setTooltip("Switch between rackspaces and songs");
    btnModeSwitch->addListener (this);  
    addAndMakeVisible (btnModeSwitch.get());

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
    container.setBounds(5, 50, 100, 500);
    container.addAndMakeVisible(highlight.get());

    for (size_t i = 0; i < buttons.size(); ++i) {
        container.addAndMakeVisible(buttons[i]);
        buttons[i]->setVisible(false);
    }

    for (size_t i = 0; i < subButtons.size(); ++i) {
        container.addAndMakeVisible(subButtons[i]);
        subButtons[i]->setVisible(false);
    }

    // ChordPro
    for (auto i = 0; i < 100; ++i) {
        std::string number = std::to_string(i);
        auto label = new Label(number, number); 
        chordProLines.add(label);
        chordProLines[i]->setEditable (false, false, false);
        chordProLines[i]->setLookAndFeel(chordProLnF);
        chordProLines[i]->setVisible(true);
        chordProContainer.addAndMakeVisible(chordProLines[i]);
    }

    // ChordPro Images
    for ( auto i = 0; i < CP_DEFAULT_IMAGES; ++i) {
        auto imageComponent = new ImageComponent(std::to_string(i));
        chordProImages.add(imageComponent);
        chordProContainer.addAndMakeVisible(chordProImages[i]);
    }

    draggableResizer.addMouseListener(this, true);
    draggableResizer.setMouseCursor(MouseCursor::LeftRightResizeCursor);
    draggableResizer.setBounds (250,50, 15, getHeight());
    containerRight.addAndMakeVisible (btnCurrent.get());
    containerRight.addAndMakeVisible (btnPrev.get());
    containerRight.addAndMakeVisible (btnNext.get());

    viewport.setBounds(5, 40, 250, 50 * buttons.size());
    viewport.setViewedComponent(&container, false);
    viewportRight.setViewedComponent(&containerRight, false);
    viewport.getVerticalScrollBar().setColour(ScrollBar::thumbColourId, Colour::fromString(BACKGROUND_COLOR));
    viewportRight.getVerticalScrollBar().setColour(ScrollBar::thumbColourId, Colour::fromString(BACKGROUND_COLOR));

    addAndMakeVisible(viewport);
    addAndMakeVisible(viewportRight);
    addAndMakeVisible(draggableResizer);
    addAndMakeVisible(fontButtonContainer);
    setSize (Rectangle<int>::fromString(DEFAULT_WINDOW_POSITION).getWidth(), 
             Rectangle<int>::fromString(DEFAULT_WINDOW_POSITION).getHeight()
            );

    extensionWindow.reset(new MyDocumentWindow());
    extensionWindow->setContentNonOwned(this, true);
    extensionWindow->setResizable(true, true);
    extensionWindow->setUsingNativeTitleBar(true);

    #if JUCE_MAC
        extensionWindow->setResizeLimits(180, 250, 10000, 10000);
    #else
        extensionWindow->getPeer()->setIcon(getWindowIcon());
        extensionWindow->setResizeLimits(200, 250, 10000, 10000);
        extensionWindow->setResizeLimits(200, 250, 10000, 10000);
    #endif
}

ExtensionWindow::~ExtensionWindow()
{
    label = nullptr;
    btnClear = nullptr;
    setLookAndFeel (nullptr);
    clockTimer.stopTimer();
}

void ExtensionWindow::paint (Graphics& g)
{
    g.fillAll (Colour::fromString(BACKGROUND_COLOR));
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
    int minWindowWidth = 180;
    int minButtonHeight = 50;
    int largeScrollAreaWidth = 50;
    Point<int> viewPos = viewport.getViewPosition();
    Point<int> viewRightPos = viewportRight.getViewPosition();
    int columns = 1;
    int buttonHeightRatio = 5; // Ratio of width:height
    auto& lnf = buttons[0]->getLookAndFeel();
    std::string lnfName = typeid(lnf).name();
    if (lnfName == typeid(gridButtonLookAndFeel).name()) {
        columns = 2;
        buttonHeightRatio = 3;
    }

    auto bounds = container.getBounds();
    bool largeScrollArea = preferences->getProperty("LargeScrollArea");
    auto buttonSize = (largeScrollArea) ? bounds.getWidth() - largeScrollAreaWidth : bounds.getWidth();
    int buttonHeight = ((int)(buttonSize/buttonHeightRatio) < minButtonHeight) ? minButtonHeight : (int)(buttonSize / buttonHeightRatio);
    auto padding = buttonHeight * 0.1;
    int rowHeight = 1;
    auto x = draggableResizer.getX();

    if (!displayRightPanel) {
       x = getWidth();
    }
    // Width of 15 provides a wider area to select the resizer on touchscreens. The displayed width is overridden in the Paint method of MyDraggableComponent.
    draggableResizer.setBounds (juce::jmax(minWindowWidth, x), 50, 15, getHeight()); 
    int buttonDisplayCount = 0;
    for (int i = 0; i < buttons.size(); ++i) {
        if (buttons[i]->isVisible()) {
            ++buttonDisplayCount;
        }
    }

    int subButtonDisplayCount = 0;
    for (size_t i = 0; i < subButtons.size(); ++i) {
        if (subButtons[i]->isVisible()) {
            ++subButtonDisplayCount;
        }
    }

    // Don't display if only 1 sub button (except if not switching immediately)
    bool switchRackSongImmediately = preferences->getProperty("SwitchImmediately");
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

    header->setBounds (0, 0, getWidth(), 50);
    clock->setBounds (getWidth()/2-50, 0, 100, 50);
    clock->setVisible(getWidth() > 450 && containerRight.isVisible() ? true : false);
    if (viewportRight.getWidth() <= 305 && viewportRight.isVisible()) fontButtonContainer.setVisible(false);

    
    auto r = header->getBounds();
    if (sidePanelOpenButton->isVisible()) {
        sidePanelOpenButton->setBounds (r.removeFromRight (50).withSizeKeepingCentre (25, 25));
    } else {
        sidePanelCloseButton->setBounds (r.removeFromRight (50).withSizeKeepingCentre (25, 25));
    }
    if (pinUnpinnedButton->isVisible()) {
        pinUnpinnedButton->setBounds (r.removeFromRight (22).withSizeKeepingCentre (25, 25));
    } else {
        r.setY(r.getY() + 5);
        pinPinnedButton->setBounds (r.removeFromRight (22).withSizeKeepingCentre (13, 13));
        r.setY(r.getY() - 5);
    }
    if (displayRightPanel) {
        if (fullscreenActivateButton->isVisible()) {
            fullscreenActivateButton->setBounds(r.removeFromRight(48).withSizeKeepingCentre (25, 25));
        } else {
            fullscreenDeactivateButton->setBounds(r.removeFromRight(48).withSizeKeepingCentre (25, 25));
        }
        lightDarkModeButton->setBounds (r.removeFromRight (45).withSizeKeepingCentre (27, 27));
        fontButton->setBounds (r.removeFromRight (45).withSizeKeepingCentre (27, 27));
    }
    
    int scrollbarBuffer = 2;
    int selectedButton = 999;
    for (size_t i = 0; i < buttons.size(); ++i) {
         buttons[i]->setBounds (buttonSize * (i % columns) + padding,
                                       buttonHeight * (i / columns) + padding + (i > selectedButton ? buttonHeight * subButtonDisplayCount : 0),
                                       buttonSize - padding - scrollbarBuffer,
                                       buttonHeight - padding);
        if (buttons[i]->getToggleState()) {  // Display sub buttons
            selectedButton = i;
            for (size_t j = 0; j < subButtonDisplayCount; ++j) {
               subButtons[j]->setBounds (buttonSize * (j % columns) + padding,
                                       buttonHeight * ((j+1) / columns) + (buttonHeight * (i / columns) + padding),
                                       buttonSize - padding - scrollbarBuffer,
                                       buttonHeight - padding);
            }
            auto highlightPadding = (buttonHeight - padding) * 0.3;
            highlight->setBounds (buttonSize * (i % columns) + padding,
                                    buttonHeight + (buttonHeight * (i / columns) + padding),
                                    buttonSize - padding - scrollbarBuffer,
                                    buttonHeight * subButtonDisplayCount - padding
            );
            highlight->getProperties().set("buttonHeight", buttonHeight - padding);
        }
    }

    container.setBounds(0, 50, juce::jmax (minWindowWidth-10, x - 10), (buttons[0]->getHeight() + padding) * rowCount );
    containerRight.setBounds(juce::jmax (minWindowWidth-10, x - 10), 50, getWidth()- juce::jmax (minWindowWidth, x), getHeight()-50);
    chordProContainer.setBounds(juce::jmax (MIN_WINDOW_WIDTH-10, x - 10), HEADER_HEIGHT, getWidth()- juce::jmax (MIN_WINDOW_WIDTH, x), getHeight()-HEADER_HEIGHT);
    fontButtonContainer.setBounds(getWidth() - 300, HEADER_HEIGHT, 290, HEADER_HEIGHT);
 
    viewport.setBounds(0, 50, juce::jmax (minWindowWidth, x), getHeight()-50);
    viewport.setViewPosition(viewPos);
    viewportRight.setBounds(juce::jmax (minWindowWidth, x), 50, getWidth() - juce::jmax (minWindowWidth, x), getHeight());

    btnCurrent->setBounds (0 , containerRight.getHeight()/4, containerRight.getWidth(), containerRight.getHeight()/2);
    btnPrev->setBounds (10 , 10, containerRight.getWidth()-10, containerRight.getHeight()/4);
    btnNext->setBounds (10 , containerRight.getHeight()*3/4, containerRight.getWidth()-10, containerRight.getHeight()/4);
    btnModeSwitch->setBounds (15, 0, getWidth() > 230 ? 120 : 60, 50);

    fontDown->setBounds (80,5,50,HEADER_HEIGHT-10);
    fontUp->setBounds (140,5,50,HEADER_HEIGHT-10);
    fontMono->setBounds (200,5,80,HEADER_HEIGHT-10);
    fontPopOverLabel->setBounds (0, 0, 80, HEADER_HEIGHT);

    draggableResizer.setVisible(displayRightPanel);
    viewportRight.setVisible(displayRightPanel);

    if (chordProForCurrentSong && viewportRight.isVisible()) {
        int runningHeight = 0;
        std::string line;
        for (auto i = 0; i < chordProLines.size(); ++i) {
            if (chordProLines[i]->isVisible()) {
                if (chordProLines[i]->getProperties()["type"] == "chordAndLyrics") {
                    rowHeight = static_cast<int>(80.0 * chordProFontSize);
                } else if (chordProLines[i]->getProperties()["type"] == "title") {
                    rowHeight = static_cast<int>(80.0 * chordProFontSize);
                } else if (chordProLines[i]->getProperties()["type"] == "label") {
                    rowHeight = static_cast<int>(50.0 * chordProFontSize);
                } else if (chordProLines[i]->getProperties()["type"] == "comment") {
                    rowHeight = static_cast<int>(50.0 * chordProFontSize);
                } else if (chordProLines[i]->getProperties()["type"] == "tab") {
                    rowHeight = static_cast<int>(30.0 * chordProFontSize);
                } else if (chordProLines[i]->getProperties()["type"] == "image") {
                    rowHeight = 0;
                    int imageIndex = chordProLines[i]->getProperties()["imageIndex"]; 
                    if (extension->chordProImages[imageIndex]->isVisible()) {
                        double originalHeight = (double)(extension->chordProImages[imageIndex]->getImage().getHeight());
                        double ratio = (double)(extension->chordProImages[imageIndex]->getImage().getWidth()) / (double)(chordProContainer.getWidth());
                        double newHeight = originalHeight / ratio;
                        rowHeight = static_cast<int>(newHeight); 
                        extension->chordProImages[imageIndex]->setBounds(0, runningHeight, chordProContainer.getWidth(), rowHeight);
                    }
                } else {
                    rowHeight = static_cast<int>(40.0 * chordProFontSize);
                }
                chordProLines[i]->setBounds(10,runningHeight,chordProContainer.getWidth(),rowHeight);
                runningHeight += rowHeight;
            } else {
                chordProLines[i]->setBounds(10,runningHeight,chordProContainer.getWidth(),0);
            }
        }
        chordProContainer.setBounds(juce::jmax (MIN_WINDOW_WIDTH-10, x - 10), HEADER_HEIGHT, getWidth()- juce::jmax (MIN_WINDOW_WIDTH, x + 10), runningHeight + 60);
    }
    viewportRight.setViewPosition(viewRightPos);
}

void ExtensionWindow::refreshUI() {
    // Reset all buttons
    for (int i = 0; i < extension->buttons.size(); ++i) {
        extension->buttons[i]->setToggleState(false, dontSendNotification);
        extension->buttons[i]->setVisible(false);
    }
    // Reset all sub buttons
    for (size_t i = 0; i < extension->subButtons.size(); ++i) {
        extension->subButtons[i]->setToggleState(false, dontSendNotification);
        extension->subButtons[i]->setVisible(false);
    }
    // Reset large previous/current/next titles
    extension->btnPrev->setButtonText("");
    extension->btnCurrent->setButtonText("");
    extension->btnNext->setButtonText("");

    if (lib->inSetlistMode()) {
            int songIndex = lib->getCurrentSongIndex();
            updateButtonLabel(SONG_TITLE);
            setTitleBarName(SONG_WINDOW_TITLE);
            updateButtonNames(lib->getSongNames());
            updateSubButtonNames(lib->getSongPartNames(songIndex));
            selectButton(lib->getCurrentSongIndex());
            selectSubButton(lib->getCurrentSongpartIndex());
            chordProReadFile(songIndex);
    } else {
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

void ExtensionWindow::toggleZeroBasedNumbering() {
    bool status = extension->preferences->getProperty("ZeroBasedNumbers");
    extension->preferences->setProperty("ZeroBasedNumbers", !status); 
    setZeroBasedNumbering(!status);
}

void ExtensionWindow::setImmediateSwitching(bool immediateSwitch) {
    extension->preferences->setProperty("ImmediateSwitching", immediateSwitch);     
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
    extension->resized();   }

void ExtensionWindow::toggleThickBorders() {
    bool status = extension->preferences->getProperty("ThickBorders");
    extension->preferences->setProperty("ThickBorders", !status); 
    refreshUI();
}

String ExtensionWindow::buttonName(int index) {
    String name = "";
    if (index < buttons.size()) {
        name = extension->buttons[index]->getButtonText();
    }
    return name;
}

bool ExtensionWindow::isButtonSelected(int index) {
    bool selected = false;
    if (index < extension->subButtons.size() && index >= 0) {
        return extension->buttons[index]->getToggleState();
    }
    return false;
}

int ExtensionWindow::getButtonSelected() {
    int selected = 0;
    for (size_t i = 0; i < extension->buttons.size(); ++i) {
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

void ExtensionWindow::selectButton(int index) {
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
        updatePrevCurrNext(index);
    }
}

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

bool ExtensionWindow::isSubButtonSelected(int index) {
    bool selected = false;
    if (index < extension->subButtons.size() && index >= 0) {
        return extension->subButtons[index]->getToggleState();
    }
    return false;
}

bool ExtensionWindow::isSubButtonsCollapsed() {
    bool collapsed = true;
    int buttonCount = extension->subButtons.size();
    if (buttonCount > 1) { // If only 1 button it will be collapsed by default
        for (size_t i = 0; i < buttonCount; ++i) {
            if (extension->subButtons[i]->isVisible()) {
                collapsed = false;
                break;
            }
        }
    }
    return collapsed;
}

void ExtensionWindow::selectSubButton(int index) {
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
    int newButtonCount = buttonNames.size();
    int currentButtonCount = extension->buttons.size();
    bool border = extension->preferences->getProperty("ThickBorders");
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
    if (lib->inSetlistMode()) {
        names = lib->getSongPartNames(index);
    } else {
        names = lib->getVariationNames(index);
    } 
    return names;
} 

void ExtensionWindow::updateButtonLnF(std::string LnFname) {
    auto& lnf = extension->buttons[0]->getLookAndFeel();
    std::string lnfName = typeid(lnf).name();

    if (lnfName == typeid(buttonLookAndFeel).name()) {
        for (size_t i = 0; i < extension->buttons.size(); ++i) {
            extension->buttons[i]->setLookAndFeel(extension->gridButtonsLnF);  
        }
    } else {
        for (size_t i = 0; i < extension->buttons.size(); ++i) {
            extension->buttons[i]->setLookAndFeel(extension->buttonsLnF);  
        }
    }
    extension->resized();
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

void ExtensionWindow::buttonClicked (Button* buttonThatWasClicked)
{
   if (buttonThatWasClicked == sidePanelOpenButton.get() || buttonThatWasClicked == sidePanelCloseButton.get())
    {
        displayRightPanel = !displayRightPanel;
        if (displayRightPanel) {
            auto bounds = draggableResizer.getBounds();
            setSize(container.getWidth() + 500, getHeight());
            sidePanelCloseButton->setVisible(true);
            sidePanelOpenButton->setVisible(false);
            fullscreenActivateButton->setVisible(true);
            clock->setVisible(true);
            if (lib->inSetlistMode() && chordProForCurrentSong) {
                extension->chordProDisplayGUI(true);
            }
        } else {
            setSize(container.getWidth() + 10, getHeight());
            sidePanelCloseButton->setVisible(false);
            sidePanelOpenButton->setVisible(true);
            fullscreenActivateButton->setVisible(false);
            extension->chordProDisplayGUI(false);
            clock->setVisible(false);
        }
        resized();
      
    } else if (buttonThatWasClicked == btnPrev.get()) {
        if (lib->inSetlistMode()) {
            bool success = lib->switchToSong(juce::jmax(0, lib->getCurrentSongIndex()-1), 0);
        } else {
            bool success = lib->switchToRackspace(juce::jmax(0, lib->getCurrentRackspaceIndex()-1));
        }
    } else if (buttonThatWasClicked == btnNext.get()) {
        if (lib->inSetlistMode()) {
            bool success = lib->switchToSong(juce::jmin(buttons.size()-1,lib->getCurrentSongIndex()+1), 0);
        } else {
            bool success = lib->switchToRackspace(juce::jmin(buttons.size()-1,lib->getCurrentRackspaceIndex()+1));
        }
    } else if (buttonThatWasClicked == pinUnpinnedButton.get() || buttonThatWasClicked == pinPinnedButton.get()) {
        bool newPinnedStatus = !(extension->extensionWindow->isAlwaysOnTop());
        pinUnpinnedButton->setVisible(!newPinnedStatus);
        pinPinnedButton->setVisible(newPinnedStatus);
        extension->extensionWindow->setAlwaysOnTop(newPinnedStatus);
        if (newPinnedStatus) {
            Rectangle<int> window = getWindowPositionAndSize();
            lib->consoleLog("Pinned GP Selector (x,y,w,h): " + std::to_string(window.getX()) + "," + std::to_string(window.getY()) + "," + std::to_string(window.getWidth()) + "," + std::to_string(window.getHeight()));
        }
        resized();
    } else if (buttonThatWasClicked == fullscreenActivateButton.get() || buttonThatWasClicked == fullscreenDeactivateButton.get()) {
        bool newFullscreenStatus = !extension->extensionWindow->isFullScreen();
        #ifdef _WIN32
            newFullscreenStatus = !(Desktop::getInstance().getKioskModeComponent() == getTopLevelComponent());
            Desktop::getInstance().setKioskModeComponent(newFullscreenStatus ? getTopLevelComponent() : nullptr, false);
            if (!newFullscreenStatus) extension->extensionWindow->getPeer()->setIcon(getWindowIcon());
        #else
            extension->extensionWindow->setFullScreen(newFullscreenStatus);
        #endif

        fullscreenActivateButton->setVisible(!newFullscreenStatus);
        fullscreenDeactivateButton->setVisible(newFullscreenStatus);
        if (newFullscreenStatus) {
            sidePanelCloseButton->setColours(Colours::grey, Colours::grey, Colours::grey);
            sidePanelCloseButton->setTooltip("Disabled while in fullscreen");
            sidePanelCloseButton->removeListener(extension);
        } else {
            sidePanelCloseButton->setColours(Colours::white, Colours::lightgrey, Colours::white);
            sidePanelCloseButton->setTooltip("Close expanded window");
            sidePanelCloseButton->addListener(extension);
        }
        resized();
    } else if (buttonThatWasClicked->getProperties()["type"] == "button") {
        bool switchRackSongImmediately = preferences->getProperty("ImmediateSwitching");
        bool inSetlist = lib->inSetlistMode();
        size_t currentGPIndex = (inSetlist ? lib->getCurrentSongIndex() : lib->getCurrentRackspaceIndex());
        int buttonIndex = buttonThatWasClicked->getProperties()["index"];
        std::vector<std::string> blank;

        // Ensure other buttons are deselected
        for (size_t i = 0; i < buttons.size(); ++i) {
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
                for (size_t i = 0; i < subButtons.size(); ++i) {
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
                bool success = lib->switchToSong(buttonIndex, 0);
            } else {
                bool success = lib->switchToRackspace(buttonIndex, 0);
            }
            updatePrevCurrNext(buttonIndex);
        }
        prevButtonSelected = buttonIndex;
    } else if (buttonThatWasClicked->getProperties()["type"] == "subButton") {
        bool switchRackSongImmediately = preferences->getProperty("ImmediateSwitching");
        int subButtonIndex = buttonThatWasClicked->getProperties()["index"];
        int buttonIndex = getButtonSelected();
        if (lib->inSetlistMode()) {
            bool success = lib->switchToSong(buttonIndex, subButtonIndex);
            std::string songpartName = lib->getSongpartName(buttonIndex, subButtonIndex);
            chordProScrollToSongPart(songpartName);
        } else {
            bool success = lib->switchToRackspace(buttonIndex, subButtonIndex);
        }
        // Ensure other buttons are toggled off
        for (size_t i = 0; i < subButtons.size(); ++i) {
            if (i != subButtonIndex && subButtons[i]->getToggleState()) {
                subButtons[i]->setToggleState(false, dontSendNotification);
            }
        }

        // Ensure large song/rackspace labels are in sync
        if (!switchRackSongImmediately) {
            updatePrevCurrNext(buttonIndex);
        }
    } else if (buttonThatWasClicked == btnModeSwitch.get()) {
        lib->inSetlistMode() ? lib->switchToPanelView() : lib->switchToSetlistView();
    } else if (buttonThatWasClicked == fontDown.get()) {
        chordProFontSize = (chordProFontSize <= CP_MIN_FONT_SIZE) ? chordProFontSize : chordProFontSize - CP_FONT_STEP_SIZE; 
        extension->resized();
    } else if (buttonThatWasClicked == fontUp.get()) {
        chordProFontSize = (chordProFontSize >= CP_MAX_FONT_SIZE) ? chordProFontSize : chordProFontSize + CP_FONT_STEP_SIZE;
        extension->resized();
    } else if (buttonThatWasClicked == fontMono.get()) {
        chordProMonospaceFont = !chordProMonospaceFont;
        extension->resized();
    } else if (buttonThatWasClicked == fontButton.get()) {
        fontButtonContainer.setVisible(!fontButtonContainer.isVisible());
        extension->resized();
    } else if (buttonThatWasClicked == lightDarkModeButton.get()) {
        chordProDarkMode = !chordProDarkMode;
        Point<int> viewRightPos = viewportRight.getViewPosition();
        extension->chordProDisplayGUI(true);
        viewportRight.setViewPosition(viewRightPos);
    }
}

void ExtensionWindow::displayWindow(bool display) {
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
    delete lib;
    lib = nullptr;

    delete extension;
    extension = nullptr;
}

void ExtensionWindow::processPreferencesDefaults(StringPairArray prefs) {
    setZeroBasedNumbering(prefs.getValue("ZeroBasedNumbers", "") == "true" ? true : false);
    extension->preferences->setProperty("ImmediateSwitching", prefs.getValue("ImmediateSwitching", "") == "false" ? false : true);
    setLargeScrollArea(prefs.getValue("LargeScrollArea", "") == "true" ? true : false);
    removeColorKeywordFromName(prefs.getValue("RemoveColorKeywordFromName", "") == "true" ? true : false); 
    StringArray positionSize = StringArray::fromTokens(prefs.getValue("PositionAndSize", DEFAULT_WINDOW_POSITION), ",", "");
    setWindowPositionAndSize(positionSize[0].getIntValue(), positionSize[1].getIntValue(), positionSize[2].getIntValue(), positionSize[3].getIntValue());
    extension->preferences->setProperty("ThickBorders", prefs.getValue("ThickBorders", "") == "true" ? true : false);
    extension->preferences->setProperty("BorderColor", prefs.getValue("BorderColor", DEFAULT_BORDER_COLOR));
}

void ExtensionWindow::processPreferencesColors(StringPairArray prefs) {
    extension->buttonColors.addArray(prefs);
}

void ExtensionWindow::processPreferencesChordPro(StringPairArray prefs) {
    extension->chordProColors.addArray(prefs);
    extension->chordProSetColors();
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

void ExtensionWindow::chordProProcessText(std::string text) {
    StringArray lines = StringArray::fromLines(text);
    String line;
    int firstLineWithContent = false;
    int imageCount = 0;
    StringArray directiveParts;
    String directiveName;
    String directiveValue;
    bool tabLine = false;
    extension->chordProReset();

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
                    } else if (directiveName == "image") {
                            extension->chordProLines[i]->getProperties().set("type", "image"); 
                            String path = directiveValue.removeCharacters("\"");
                            #ifdef _WIN32
                                path = path.replaceCharacters("\\", "\\\\");
                            #endif
                            auto file = File(path);
                            Image image = ImageFileFormat::loadFrom(file);
                            if (image.isValid()) {
                                extension->chordProImagesCheckAndAdd(imageCount);
                                extension->chordProImages[imageCount]->setImage(image);
                                extension->chordProImages[imageCount]->setImagePlacement (RectanglePlacement (RectanglePlacement::fillDestination | RectanglePlacement::xLeft | RectanglePlacement::yTop));
                                extension->chordProImages[imageCount]->setVisible(true);
                                extension->chordProLines[i]->getProperties().set("imageIndex", imageCount);
                                imageCount++;
                            }
                            directiveValue = "";
                    } else {
                        extension->chordProLines[i]->setLookAndFeel(extension->chordProSubTitleLnF);
                        extension->chordProLines[i]->getProperties().set("type", "subtitle"); 
                        directiveValue = directiveName.substring(0,1).toUpperCase() + directiveName.substring(1,directiveName.length()) + ": " + directiveValue;
                    } 
                } else {
                    extension->chordProLines[i]->setVisible(false);
                    directiveName = line.removeCharacters(" ");
                }
                if (directiveName == "start_of_tab" || directiveName == "sot") {
                    tabLine = true;
                } else if (directiveName == "end_of_tab" || directiveName == "eot") {
                    tabLine = false;
                }
                extension->chordProLines[i]->setText(directiveValue, dontSendNotification);
            } else {
                if (tabLine) {
                    extension->chordProLines[i]->setLookAndFeel(extension->chordProTabLnF);
                    extension->chordProLines[i]->getProperties().set("type", "tab");
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
                }
                extension->chordProLines[i]->setText(line.trim(), dontSendNotification);
            }
        }
    }
    extension->resized();
}

void ExtensionWindow::chordProReadFile(int index) {
    char *homePath;
    std::string chordProPath;
    std::string chordProFileText;
    std::string chordProFile = lib->getChordProFilenameForSong(index);
    extension->chordProForCurrentSong = (chordProFile == "") ? false : true;
    extension->chordProDisplayGUI(extension->chordProForCurrentSong);
    if (!extension->chordProForCurrentSong) {
        extension->chordProReset();
        return;
    }
    if ((homePath = getenv(ENVIRONMENT_VARIABLE())) != NULL) {
        std::string s;
        std::stringstream ss;
        ss << homePath;
        ss >> s;
        chordProPath = s + LYRICS_CHORDS_PATH() + chordProFile;
        #ifdef _WIN32
            String replaced = chordProPath;
            replaced = replaced.replaceCharacter('/', PATH_SEPARATOR());
            chordProPath = replaced.toStdString();
        #endif
        gigperformer::sdk::GPUtils::loadTextFile(chordProPath, chordProFileText);
        chordProProcessText(chordProFileText);   
    }
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
    }
}

void ExtensionWindow::chordProScrollToSongPart(std::string songPartName) {
    int viewY;
    int btnY;
    Rectangle<int> viewportBounds = extension->viewportRight.getViewArea();
    for (int i = 0; i < extension->chordProLines.size(); ++i) { 
        if (extension->chordProLines[i]->getProperties()["type"] == "gp_songpartname") {
            if (extension->chordProLines[i]->getText().toStdString() == songPartName) {
                Rectangle<int> buttonBounds = extension->chordProLines[i]->getBounds();
                viewY = viewportBounds.getY() + viewportBounds.getHeight();
                btnY = buttonBounds.getY() + buttonBounds.getHeight();
                extension->viewportRight.setViewPosition(0, buttonBounds.getY());
            }
        }
    }
}

void ExtensionWindow::chordProDisplayGUI(bool display) { 
    fontButton->setVisible(display && displayRightPanel);
    lightDarkModeButton->setVisible(display && displayRightPanel);
    if (!display) fontButtonContainer.setVisible(false);
    if (display) { 
        extension->viewportRight.setViewedComponent(&extension->chordProContainer, false);
        extension->chordProSetColors();
    } else {
        if (!chordProForCurrentSong) {
            extension->viewportRight.setViewedComponent(&extension->containerRight, false);
            viewPortBackground = Colour::fromString(BACKGROUND_COLOR);
        }
    }
    extension->viewportRight.setViewPosition(0,0);
    extension->resized();
}

void ExtensionWindow::chordProSetColors() { 
    if (chordProDarkMode) {
        chordProLyricColor = Colour::fromString(extension->chordProColors.getValue("ChordProDarkModeColorsLyrics",CP_DARK_LYRIC_COLOR));
        chordProChordColor = Colour::fromString(extension->chordProColors.getValue("ChordProDarkModeColorsChords",CP_DARK_CHORD_COLOR));
        viewPortBackground = Colour::fromString(extension->chordProColors.getValue("ChordProDarkModeColorsBackground",CP_DARK_BACKGROUND_COLOR));
    } else {
        chordProLyricColor = Colour::fromString(extension->chordProColors.getValue("ChordProLightModeColorsLyrics",CP_LIGHT_LYRIC_COLOR));
        chordProChordColor = Colour::fromString(extension->chordProColors.getValue("ChordProLightModeColorsChords",CP_LIGHT_CHORD_COLOR));
        viewPortBackground = Colour::fromString(extension->chordProColors.getValue("ChordProLightModeColorsBackground",CP_LIGHT_BACKGROUND_COLOR));
    }
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

void MyDocumentWindow::closeButtonPressed () { 
    ExtensionWindow::displayWindow(false);
}

void ClockTimer::timerCallback() {
    ExtensionWindow::updateClock(Time::getCurrentTime().toString(false, true, false, true));
}

void RefreshTimer::timerCallback() {
    ExtensionWindow::compareButtonNames(lib->inSetlistMode() ? lib->getSongNames() : lib->getRackspaceNames());
    ExtensionWindow::compareSubButtonNames(lib->inSetlistMode() ? lib->getSongPartNames(ExtensionWindow::getButtonSelected()) : lib->getVariationNames(ExtensionWindow::getButtonSelected()));
}

#if 0

#endif

