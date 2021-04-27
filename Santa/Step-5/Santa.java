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

	public void wakeUpSanta(int elvesOrReindeers) {
		if (elvesOrReindeers == 0) {
			this.state = SantaState.WOKEN_UP_BY_ELVES;
		}
		else {
			this.state = SantaState.WOKEN_UP_BY_REINDEER;
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
					this.scenario.fixingProblems.acquire();

					while (!this.scenario.elvesAtSantasDoor.isEmpty()) {
						this.scenario.elvesAtSantasDoor.remove().setState(Elf.ElfState.WORKING);
					}
					
					this.state = SantaState.SLEEPING;
					this.scenario.fixingProblems.release();
				} catch (Exception e) {
					e.printStackTrace();
				}

				this.scenario.inTrouble.release(3);
				break;
			case WOKEN_UP_BY_REINDEER:
				for (Reindeer reindeer: this.scenario.reindeer) {
					reindeer.setState(Reindeer.ReindeerState.AT_THE_SLEIGH);
					this.scenario.waitForLastReindeer.release();
				}
				this.state = SantaState.READY_FOR_CHRISTMAS;
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
