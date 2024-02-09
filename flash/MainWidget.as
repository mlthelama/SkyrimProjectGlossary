class MainWidget extends MovieClip
{
	/* PRIVATE VARIABLES */
	private var dataObject: Object;
    
	var LB_Hint: MovieClip;
	var RB_Hint: MovieClip;
	
	function MainWidget() {
		super();
		LB_Hint.ButtonArt_MC.gotoAndStop(274);
		RB_Hint.ButtonArt_MC.gotoAndStop(275);
	}

	public function get dataProvider(): Object
	{
		return dataObject;
	}
    
	public function set dataProvider(value: Object): Void
	{
		if (dataObject == value) {
			return;
		}
        
        dataObject = value;

		if (dataObject == null) {
			return;
		}
        
		var buttonRight: Number = dataObject.buttonRight != null ? dataObject.buttonRight : 0;
		var buttonLeft: Number = dataObject.buttonLeft != null ? dataObject.buttonLeft : 0;
                
        RB_Hint.ButtonArt_MC.gotoAndStop(buttonRight);
        LB_Hint.ButtonArt_MC.gotoAndStop(buttonLeft);
    }
}