import gfx.controls.Button;
import gfx.events.EventTypes;
import gfx.io.GameDelegate;

class GlobalListener extends MovieClip {
	/* PRIVATE VARIABLES */


	/* INITIALIZATION */
	public function GlobalListener() {
		super();
        log("Running Listener ...");
	}


	public function onLoad(): Void {

	}


	private function log(a_message: String): Void {
		GameDelegate.call("Log", [a_message]);
	}
}
