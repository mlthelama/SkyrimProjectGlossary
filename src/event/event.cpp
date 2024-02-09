#include "event.h"
#include "event/input_event.h"

namespace event {
    void event::sink_event_handlers() {
        logger::info("adding event sinks ..."sv);

        input_event::sink();
        
        logger::info("added event sinks"sv);
    }
}
