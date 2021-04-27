import java.util.Random;

public class Elf implements Runnable {

	enum ElfState {
		WORKING, TROUBLE, AT_SANTAS_DOOR
	};

	private ElfState state;
	/**
	 * The number associated with the Elf
	 */
	private int number;
	private Random rand = new Random();
	private SantaScenario scenario;
	private boolean exit = false;

	public Elf(int number, SantaScenario scenario) {
		this.number = number;
		this.scenario = scenario;
		this.state = ElfState.WORKING;
	}


	public ElfState getState() {
		return state;
	}

	/**
	 * Santa might call this function to fix the trouble
	 * @param state
	 */
	public void setState(ElfState state) {
		this.state = state;
	}


	@Override
	public void run() {
		while (!this.exit) {
      // wait a day
  		try {
  			Thread.sleep(100);
  		} catch (InterruptedException e) {
  			e.printStackTrace();
  		}
			switch (state) {
			case WORKING: {
				// at each day, there is a 1% chance that an elf runs into
				// trouble.
				if (rand.nextDouble() < 0.01) {
					try {
						// We want to try and acquire the permit here because doing
						// so in the TROUBLE case could cause one thread to grab all
						// semaphores.
						this.scenario.inTrouble.acquire();
						state = ElfState.TROUBLE;
						this.scenario.elvesInTrouble.add(this);
					} catch (Exception e) {
						e.printStackTrace();
					}
				}
				break;
			}
			case TROUBLE:
				// Here, the three elves will be in trouble and essentially just
				// waiting to get sent to Santa's door so he can fix their problems
				break;
			case AT_SANTAS_DOOR:
				this.scenario.santa.wakeUpSanta();
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
		System.out.println("Elf " + number + " : " + state);
	}
}
