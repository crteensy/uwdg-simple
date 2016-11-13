#ifndef UWDG_BUTTON_H
#define UWDG_BUTTON_H

#include "label.h"

/* removed in "simple" version
#include <Signals/Signals.h>
*/

namespace uwdg
{
class Button : public Label
{
public:
  Button(Widget* parent = nullptr) :
    Label(parent),
    onClicked_(nullptr)
  {
    setAcceptsFocus(true);
  }
  Button(const char* s, Widget* parent = nullptr)  :
    Label(s, parent)
  {
    setAcceptsFocus(true);
    setAlignment(Alignment::center);
  }
  void draw() const override
  {
    Label::draw();
    PRINTDEBUG(("Button(@%p)::draw()\n", this));
  }
  void onInputEvent(InputEvent& event)
  {
    if (event.isPress() && (event.type() == InputEvent::eEnter))
    {
//      Serial.println("Button clicked");
      event.accept();
      //clicked();
      if(onClicked_ != nullptr)
      {
        onClicked_();
			}
    }
  }
  /* removed in "simple" version
  Signal<> clicked;
  */
  typedef void (*TCallback)();
  void setOnClicked(TCallback f)
	{
		onClicked_ = f;
	}
protected:

private:
  TCallback onClicked_;
};

} // namespace uwdg

#endif // UWDG_BUTTON_H

