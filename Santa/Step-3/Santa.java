//import com.sun.org.apache.xml.internal.security.utils.HelperNodeList;

public class Santa implements Runnable {

	enum SantaState {SLEEPING, READY_FOR_CHRISTMAS, WOKEN_UP_BY_ELVES, WOKEN_UP_BY_REINDEER};
	private SantaState state;
	private SantaScenario scenario;
	private boolean exit = false;
	
	public Santa(SantaScenario scenario) {
		this.state = SantaState.SLEEPING;
		this.scenario = scenario;
	}

	public void wakeUpSanta() {
		if (this.state != SantaState.WOKEN_UP_BY_ELVES) {
			this.state = SantaState.WOKEN_UP_BY_ELVES;
		}
	}
	
	@Override
	public void run() {
		while(!this.exit) {
			// wait a day...
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			switch(state) {
			case SLEEPING: // if sleeping, continue to sleep
				break;
			case WOKEN_UP_BY_ELVES:
				// If Santa is woken up by elves, he will solve the problems
				// of three elves at his door, and go back to sleeping
				while (!this.scenario.atSantasDoor.isEmpty()) {
					this.scenario.atSantasDoor.remove().setState(Elf.ElfState.WORKING);
				}
				this.state = SantaState.SLEEPING;
				break;
			case WOKEN_UP_BY_REINDEER: 
				break;
			case READY_FOR_CHRISTMAS: // nothing more to be done
				break;
			}
		}
	}

	public void endThread() {
		this.exit = true;
	}
	
	/**
	 * Report about my state
	 */
	public void report() {
		System.out.println("Santa : " + state);
	}
	
	
}
