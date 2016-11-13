#ifndef INPUTEVENT_H
#define INPUTEVENT_H

class InputEvent
{
	public:
    enum EInputType
    {
      eLeft,
      eUp,
      eRight,
      eDown,
      eEnter,
      eCW,
      eCCW
    };
		InputEvent(const EInputType& inputType, bool down)
      : inputType_(inputType),
      down_(down),
      accepted_(false)
		{
//		  Serial.printf("InputEvent(%d, %d)\n", type(), isPress());
		}

		~InputEvent()
		{
		  if (!accepted())
		  {
//		    Serial.printf("Unhandled InputEvent(%d, %d)\n", type(), isPress());
		  }
		}

    /** Get input type of this event **/
    const EInputType& type() const {return inputType_;}

		/** Accept the event and mark it as accepted **/
		void accept() {accepted_ = true;}

		/** Was the event accepted? **/
		bool accepted() const {return accepted_;}

    /** Was this a button press event? **/
		bool isPress() const {return down_;}

    /** Was this a button release event? **/
		bool isRelease() const {return !down_;}

//		static Signal<InputEvent&> onInput()
//		{
//		  static Signal<InputEvent&> sig;
//		  return sig;
//		}

	protected:
		const EInputType inputType_;
		const bool down_;
		bool accepted_;

	private:

};

#endif // INPUTEVENT_H
