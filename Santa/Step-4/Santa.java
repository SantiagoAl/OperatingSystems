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
				try {
					// Using a semaphore here will help us to not prematurely start fixing
					// elves problems and go back to sleep before the three elves problems
					// are fixed
					this.scenario.fixingElvesProblems.acquire();

					while (!this.scenario.atSantasDoor.isEmpty()) {
						this.scenario.atSantasDoor.remove().setState(Elf.ElfState.WORKING);
					}
					
					// Now, Santa can go back to sleep
					this.state = SantaState.SLEEPING;
					this.scenario.fixingElvesProblems.release();
				} catch (Exception e) {
					e.printStackTrace();
				}

				// Release the semaphore so that other elves that are now in trouble
				// can get put into the TROUBLE queue
				this.scenario.inTrouble.release(3);
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
