// Rackspace and song selector extension for Gig Performer by @rank13

#pragma once

#include "Constants.h"

using namespace juce;

extern Colour chordProLyricColor;
extern Colour chordProChordColor;
extern bool chordProMonospaceFont;

class buttonLookAndFeel : public LookAndFeel_V4 {
public:
	void drawButtonText (Graphics& g, TextButton& button,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{
		Font font (button.getHeight () * 0.25f);
		g.setFont (font);
		g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
			: TextButton::textColourOffId)
      .withMultipliedAlpha (0.5f));
    
    const auto buttonWidth = (double) button.getWidth();
    const auto buttonHeight = (double) button.getHeight();
    const String buttonText = button.getButtonText();
    const auto yIndent = buttonHeight * 0.1;
		const auto leftIndent = buttonWidth > 160 ? yIndent * 2 : 5;
    auto availableWidth = buttonWidth - leftIndent;
    const int rows = 1;
		
    // Button number
    Font font1 (juce::jmax(button.getHeight () * 0.4f, 16.f));
    g.setFont (font1);
    auto buttonNumber = button.getProperties()["displayIndex"];
    auto numberWidth = font1.getStringWidthFloat(button.getName()); 
    g.drawFittedText (buttonNumber,
      leftIndent, yIndent, numberWidth, buttonHeight - yIndent * 2,
      Justification::right, rows, 0.5f);

    // Button Name
    Font font2 (juce::jmax(button.getHeight () * 0.4f, 16.f));
    g.setFont (font2);
    g.setColour (button.getToggleState () ? Colours::white : Colour(0xffc5c5c5));
    g.drawFittedText (buttonText,
      leftIndent + (numberWidth * 1.5), yIndent, availableWidth - (numberWidth * 1.5), buttonHeight - yIndent * 2,
      Justification::left, rows, 1.0f);
	}

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool isButtonHighlighted, bool isButtonDown) {
    auto buttonArea = button.getLocalBounds().toFloat();
    float borderSize = buttonArea.getHeight() * ((button.getProperties()["thickBorder"]) ? 0.08 : 0.04);
    float cornerSize = buttonArea.getHeight() * 0.08;
    Colour buttonColor = Colour::fromString(button.getProperties()["colour"].toString());

    if (button.getToggleState()) {
      g.setColour (Colour(0xff6a6a6a));
    } else if (isButtonHighlighted && !isButtonDown) {
      g.setColour (Colour(0xff2f2f2f));
    } else if (isButtonDown) {
      g.setColour (Colour(0xff9a9a9a));
    } else {
      g.setColour (buttonColor);
    }   
    g.fillRoundedRectangle (buttonArea, cornerSize);
    if (button.getToggleState()) {
      g.setColour (Colours::white);
      buttonArea = buttonArea.withSizeKeepingCentre(buttonArea.getWidth() - borderSize, buttonArea.getHeight() - borderSize);      
      g.drawRoundedRectangle (buttonArea, cornerSize, borderSize);  
    }               
  }
};

class subButtonLookAndFeel : public LookAndFeel_V4 {
public:
	void drawButtonText (Graphics& g, TextButton& button,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{
		Font font (button.getHeight () * 0.25f);
		g.setFont (font);
		g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
			: TextButton::textColourOffId)
      .withMultipliedAlpha (0.5f));
    const auto buttonWidth = (double) button.getWidth();
    const auto buttonHeight = (double) button.getHeight();
    const auto padding = buttonHeight * 0.3;
    const String buttonText = button.getButtonText();
    const auto yIndent = buttonHeight * 0.1;
		const auto leftIndent = buttonWidth > 160 ? padding + (yIndent * 4) : padding + 5;

    // Button Name
    Font font2 (juce::jmax(buttonHeight * 0.4, 16.0));
    g.setFont (font2);
    g.setColour (button.getToggleState() ? Colours::white : Colour(0xffe5e5e5));
    auto availableWidth = buttonWidth - leftIndent;
    g.drawFittedText (buttonText,
      leftIndent, yIndent, availableWidth, buttonHeight - yIndent * 2,
      Justification::left, 1, 1.0f);
	}

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool isButtonHighlighted, bool isButtonDown) {
    auto buttonArea = button.getLocalBounds().toFloat();
    auto highlightArea = button.getLocalBounds().toFloat();;
    auto padding = buttonArea.getHeight() * 0.3;
    buttonArea.setLeft(padding);
    float borderSize = buttonArea.getHeight() * (button.getProperties()["thickBorder"] ? 0.08 : 0.04);
    float cornerSize = buttonArea.getHeight() * 0.08;
    Colour buttonColor = Colour::fromString(button.getProperties()["colour"].toString());
    Colour borderColour = Colour::fromString(button.getProperties()["borderColor"].toString());

    if (button.getToggleState()) {
      if (buttonColor == Colour::fromString(DEFAULT_SUBBUTTON_COLOR)) {
        g.setColour (Colour(0xff6A6A6A));
      } else {
        g.setColour (buttonColor);
      }      
    } else if (isButtonHighlighted && !isButtonDown) {
      if (buttonColor == Colour::fromString(DEFAULT_SUBBUTTON_COLOR)) {
        g.setColour (buttonColor.withBrightness(buttonColor.getBrightness() + 0.1f));
      } else {
        g.setColour (buttonColor.withBrightness(buttonColor.getBrightness() - 0.1f));
      }
    } else if (isButtonDown) {
      g.setColour (Colour(0xff9a9a9a));
    } else {
      g.setColour (buttonColor);
    }   
    g.fillRoundedRectangle (buttonArea, cornerSize);  
    if (button.getToggleState()) {
      g.setColour (borderColour);
      buttonArea = buttonArea.withSizeKeepingCentre(buttonArea.getWidth() - borderSize, buttonArea.getHeight() - borderSize);
      g.drawRoundedRectangle (buttonArea, cornerSize, borderSize);  

      // Button Highlight
      highlightArea.setX(padding/4.0);
      highlightArea.setWidth(padding/6.0);
      g.setColour (Colours::white);
      g.fillRoundedRectangle (highlightArea, borderSize);  
    }            
  }
};

class subButtonHighlightLookAndFeel : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    auto labelArea = label.getLocalBounds().toFloat();
    auto highlightArea = label.getLocalBounds().toFloat();
    auto padding = (float)label.getProperties()["buttonHeight"] * 0.3;
    float cornerSize = 5.f;
    g.setColour (Colours::grey);
    highlightArea.setX(padding/4.0);
    highlightArea.setWidth(padding/6.0);
    g.fillRoundedRectangle (highlightArea, cornerSize);
  }
};

class blankButtonLookAndFeel : public LookAndFeel_V4 {
public:
	void drawButtonText (Graphics& g, TextButton& button,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {};

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                               bool isButtonHighlighted, bool isButtonDown) {};
};

class gridButtonLookAndFeel : public LookAndFeel_V4 {
public:
	void drawButtonText (Graphics& g, TextButton& button,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{
		Font font (button.getHeight () * 0.25f);
		g.setFont (font);
		g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
			: TextButton::textColourOffId)
      .withMultipliedAlpha (0.5f));
    
		const int yIndent = button.proportionOfHeight (0.1f) * 2;
		const int cornerSize = jmin (button.getHeight (), button.getWidth ()) / 2;
		const int leftIndent = cornerSize / (button.isConnectedOnLeft () ?
                  yIndent * 2 : yIndent);
		const int rightIndent = cornerSize / (button.isConnectedOnRight () ? 
                  yIndent * 2 : yIndent);
		const int textWidth = button.getWidth () - leftIndent - rightIndent;
    const int rows = 1;
		if (textWidth > 100) {
      // Button number
      Font font1 (juce::jmax(button.getHeight () * 0.4f, 16.f));
		  g.setFont (font1);
      int numberWidth = (int) font1.getStringWidthFloat(button.getName());  // Base width on 2 digits
      auto buttonNumber = button.getProperties()["displayIndex"];
      g.drawFittedText (buttonNumber,
      leftIndent*4, yIndent, numberWidth, button.getHeight () - yIndent * 2,
      Justification::right, rows, 0.5f);
      
      // Button Name
      Font font2 (juce::jmax(button.getHeight () * 0.4f, 16.f));
		  g.setFont (font2);
      g.setColour  (button.getToggleState () ? Colours::white : Colour(0xffc5c5c5));
			g.drawFittedText (button.getButtonText (),
				(leftIndent*4) + (numberWidth*1.5), yIndent, textWidth-(leftIndent*20), button.getHeight () - yIndent * 2,
				Justification::left, rows, 1.0f);
    }
	}

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool isButtonHighlighted, bool isButtonDown) {
    auto buttonArea = button.getLocalBounds().toFloat().reduced (0.5f);
    float cornerSize = 5.f;
    Colour myColour = Colour(0xff1f1f1f);
    String btnColour = button.getProperties()["colour"];
    myColour = Colour::fromString(btnColour);

    if (button.getToggleState()) {
      g.setColour (Colour(0xff6a6a6a));
      
    } else if (isButtonHighlighted && !isButtonDown) {
      g.setColour (Colour(0xff2f2f2f));
    } else if (isButtonDown) {
      g.setColour (Colour(0xff9a9a9a));      
    } else {
      g.setColour (myColour);
      
    }   
    g.fillRoundedRectangle (buttonArea, cornerSize);
    if (button.getToggleState()) {
      g.setColour (Colours::red);
      g.drawRoundedRectangle (buttonArea, cornerSize, 1.f);  
    }               
  }
};

class headerLookAndFeel : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    auto labelArea = label.getLocalBounds();
    g.setFont (Font (25.00f, Font::plain).withTypefaceStyle ("Regular"));
    g.setColour (Colours::white);
    g.drawFittedText (label.getText(),
				0, 0, label.getWidth(), label.getHeight (),
				Justification::centred, 1, 1.0f);
  }
};

class headerSongs : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    g.fillAll (Colour (0xff894B28));
    auto labelArea = label.getLocalBounds();
    g.setFont (Font (25.00f, Font::plain).withTypefaceStyle ("Regular"));
    g.setColour (Colours::white);
    g.drawFittedText ("Songs",
				15, 0, label.getWidth(), label.getHeight (),
				Justification::centredLeft, 1, 1.0f);
  }
};

class headerRackspaces : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    g.fillAll (Colour (0xff2c5e1e));
    auto labelArea = label.getLocalBounds();
    g.setFont (Font (25.00f, Font::plain).withTypefaceStyle ("Regular"));
    g.setColour (Colours::white);
    String title = (labelArea.getWidth() < 240 ? "Racks" : "Rackspaces");
    g.drawFittedText (title,
				15, 0, label.getWidth(), label.getHeight (),
				Justification::centredLeft, 1, 1.0f);
  }
};

class minimalistSong : public LookAndFeel_V4 {
public:
	void drawButtonText (Graphics& g, TextButton& button,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{
		const int yIndent = button.proportionOfHeight (0.1f);
		const int cornerSize = jmin (button.getHeight (), button.getWidth ()) / 2;
		const int leftIndent = cornerSize / (button.isConnectedOnLeft () ?
                  yIndent * 2 : yIndent);
		const int rightIndent = cornerSize / (button.isConnectedOnRight () ? 
                  yIndent * 2 : yIndent);
		const int textWidth = button.getWidth () - leftIndent - rightIndent;

    g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
      : TextButton::textColourOffId)
      .withMultipliedAlpha (button.isEnabled () ? 1.0f : 0.5f));
		if (textWidth > 100) {
      if (button.getName() == "btnCurrent") {
    		Font font (button.getHeight () * 0.5f);
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
          : TextButton::textColourOffId)
          .withMultipliedAlpha (1.0f));
      }
      else if (button.getName() == "btnNext") {
        Font font (button.getHeight () * 0.5f);
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
          : TextButton::textColourOffId)
          .withMultipliedAlpha (0.8f));
      } else {
        Font font (button.getHeight () * 0.5f);
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
          : TextButton::textColourOffId)
          .withMultipliedAlpha (0.5f));
      }
      g.drawFittedText (button.getButtonText (),
				leftIndent, yIndent, textWidth, button.getHeight () - yIndent * 2,
				Justification::centred, 2, 0.8f);
    }
	}

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                               bool isButtonHighlighted, bool isButtonDown) {
     if (button.getName() == "btnCurrent") {
        g.fillAll (Colour (0xff1C1C1C));
     }
  }
};

class chordPro : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    StringArray wordParts;
    StringArray finalText;
    bool priorPartIsChord;
    bool successiveChords;
    double lastWordPosition = 0.0;
    double lastChordPosition = 0.0;
    double lastPosition = 0.0;
    double nextWordPosition = 0.0;
    int leftPad = 50;
    auto labelArea = label.getLocalBounds();

    Font font (Font (label.getHeight(), Font::plain));
    Font chordFont (Font (label.getHeight(), Font::plain));

    if (chordProMonospaceFont) {
      font.setTypefaceName(Font::getDefaultMonospacedFontName());
      chordFont.setTypefaceName(Font::getDefaultMonospacedFontName());      
    } else {
      font = font.withTypefaceStyle("Regular").withExtraKerningFactor(0.06f);
      chordFont = chordFont.withTypefaceStyle("Regular");
    }
    if (label.getProperties()["type"] == "chordAndLyrics") {
      font = font.withHeight(label.getHeight() * 0.5f);
      chordFont = chordFont.withHeight(label.getHeight() * 0.5f);
    } 
    g.setFont (chordFont);
    StringArray words = StringArray::fromTokens(label.getText(),false);
    for (int i = 0; i < words.size(); ++i) { 
      priorPartIsChord = false;
      successiveChords = false;
      wordParts = StringArray::fromTokens(words[i],"[]","");
      if (i==0 && !(label.getProperties()["type"] == "chordOnly")) wordParts.removeEmptyStrings();
      String chordCheck;
      String chord;

      for (int j = 0; j < wordParts.size(); ++j) {
        chordCheck = "["+wordParts[j]+"]";
        if (words[i].contains(chordCheck) || label.getProperties()["type"] == "chordOnly") { // Is a chord    
          chord = wordParts[j];
          successiveChords = priorPartIsChord;
          priorPartIsChord = true;
          g.setFont (chordFont);
          g.setColour(chordProChordColor);
          if (lastWordPosition < lastChordPosition) {
            lastWordPosition = lastChordPosition;
          } 
          g.drawFittedText (chord,
          lastWordPosition + leftPad, 2, label.getWidth(), label.getHeight(),
          Justification::topLeft, 1, 1.0f);
          lastChordPosition = lastWordPosition + font.getStringWidthFloat(wordParts[j]);// + (label.getHeight() / 4);
          if (label.getProperties()["type"] == "chordAndLyrics") lastChordPosition = lastChordPosition + font.getStringWidthFloat(" "); // Force minimum gap
          lastPosition += font.getStringWidthFloat(wordParts[j]);
        } else {
          g.setColour (chordProLyricColor);
          g.setFont (font);
          g.drawFittedText ( wordParts[j],
            lastWordPosition + leftPad, 0, label.getWidth(), label.getHeight(),
            Justification::bottomLeft, 1, 1.0f);
          chord = "";
          priorPartIsChord = false;
          lastWordPosition += font.getStringWidthFloat(wordParts[j]);
          lastPosition += font.getStringWidthFloat(wordParts[j]);
        }
      }
      lastWordPosition += font.getStringWidthFloat(" "); 
    }
  }
};

class chordProTitle : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    auto labelArea = label.getLocalBounds();
    int leftPad = 50;
    g.setFont (Font (label.getHeight() * 0.85f, Font::plain).withTypefaceStyle ("Regular")); //.boldened());
    g.setColour (chordProLyricColor);
    g.drawFittedText (label.getText(),
				leftPad, 0, label.getWidth(), label.getHeight (),
				Justification::centredLeft, 1, 1.0f);
    g.setColour (Colours::white);
  }
};

class chordProSubTitle : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    auto labelArea = label.getLocalBounds();
    int leftPad = 50;
    g.setFont (Font (label.getHeight(), Font::plain).withTypefaceStyle ("Regular").italicised());
    g.setColour (chordProLyricColor.withAlpha(0.8f));
    g.drawFittedText (label.getText(),
				leftPad, 0, label.getWidth(), label.getHeight (),
				Justification::centredLeft, 1, 1.0f);
    g.setColour (Colours::white);
  }
};

class chordProComment : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    int leftPad = 50;
    auto labelArea = label.getLocalBounds();
    g.setColour (Colours::lightgrey.withAlpha(0.3f));
    Font font (Font (label.getHeight() * 0.8f, Font::plain).withTypefaceStyle ("Regular").italicised());
    g.setFont (font);
    auto textWidth = (int) font.getStringWidthFloat(label.getText());
    auto textHeight = (int) font.getHeight();
    auto y = (int) (labelArea.getHeight() - textHeight) / 2;
    g.fillRect( leftPad - 10, y - 5, textWidth + 25, textHeight + 10);
    g.setColour (chordProLyricColor.withAlpha(0.8f));
    g.drawFittedText (label.getText(),
				leftPad, y, label.getWidth(), textHeight,
				Justification::centredLeft, 1, 1.0f);
    g.setColour (Colours::white);
  }
};

class chordProLabel : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    auto labelArea = label.getLocalBounds();
    int leftPad = 50;
    Font font ((Font (label.getHeight(), Font::plain).withTypefaceStyle ("Regular").boldened()));
    g.setFont (font);
    g.setColour (chordProLyricColor.withAlpha(0.9f));
    g.drawFittedText (label.getText(),
				leftPad, 0, label.getWidth(), label.getHeight (),
				Justification::centredLeft, 1, 1.0f);
    g.setColour (Colours::white);
    auto textWidth = (int) font.getStringWidthFloat(label.getText());
    g.setColour (Colours::grey);
    g.drawLine(leftPad, labelArea.getBottom(), leftPad + textWidth, labelArea.getBottom(), 3);
  }
};

class chordProTab : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    auto labelArea = label.getLocalBounds();
    int leftPad = 50;
    Font font (Font (label.getHeight(), Font::plain));
    font.setTypefaceName(Font::getDefaultMonospacedFontName());
    g.setFont (font);
    auto textWidth = (int) font.getStringWidthFloat(label.getText());
    auto textHeight = (int) font.getHeight();
    g.setColour (Colours::grey.withAlpha(0.08f));
    g.fillRect( leftPad, 0, textWidth, textHeight);
    String text = label.getText().toStdString();
    int runningTextWidth = 0;
    for (int i = 0; i < text.length(); i++) {
      g.setColour (chordProLyricColor.withAlpha(0.8f));
      runningTextWidth = (int) font.getStringWidthFloat(text.substring(0, i));
      if (text.substring(i, i+1) == "-" || text.substring(i, i+1) == "|") {
        g.setColour (chordProLyricColor.withAlpha(0.3f));
      }
      g.drawFittedText (text.substring(i, i+1),
        leftPad + runningTextWidth, 0, label.getWidth(), label.getHeight (),
        Justification::centredLeft, 1, 1.0f);
    }
  }
};

class popOverLookAndFeel : public LookAndFeel_V4 {
public:
  void drawButtonText (Graphics& g, TextButton& button,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{
		Font font (button.getHeight() * ((button.getName() == "Mono" || button.getName() == "Create") ? 0.5 : 0.9));
    auto height = ((button.getName() == "Mono" || button.getName() == "Create") ? button.getHeight() : button.getHeight() * 0.85);
		g.setFont (font);
		g.setColour (Colours::white);
    g.drawFittedText (button.getButtonText(),
				0, 0, button.getWidth(), height,
				Justification::centred, 1, 1.0f);
    }

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool isButtonHighlighted, bool isButtonDown) {
    auto buttonArea = button.getLocalBounds().toFloat().reduced (0.5f);
    float cornerSize = 5.f;
    if (button.getToggleState()) {
      g.setColour (Colour(0xff6a6a6a));
      
    } else if (isButtonHighlighted && !isButtonDown) {
      g.setColour (Colours::darkgrey);
    } else if (isButtonDown) {
      g.setColour (Colour(0xff9a9a9a));
    } else {
      g.setColour (Colour(0xff2f2f2f));
    }   
    g.fillRoundedRectangle (buttonArea, cornerSize);
    g.setColour(Colours::white.withAlpha(0.1f));
    g.drawRoundedRectangle (buttonArea, cornerSize, 2.f);       
  }
};

class popOverLabel : public LookAndFeel_V4 {
public:
	void drawLabel (Graphics& g, Label& label) {
		const int yIndent = label.proportionOfHeight (0.2f);
		const int textWidth = label.getWidth();
    g.setFont (Font (25.00f, Font::plain).withTypefaceStyle ("Regular"));
    g.setColour (chordProLyricColor.withMultipliedBrightness(0.8f));
    g.drawFittedText (label.getText(),
      15, 0, textWidth, label.getHeight(),
      Justification::left, 1, 1.0f);
    }
};
