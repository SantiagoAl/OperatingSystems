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
	private boolean amInTrouble = false;

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
					state = ElfState.TROUBLE;
				}
				break;
			}
			case TROUBLE:
				// Here, we will only add the elf to the 'elvesInTrouble'
				// Queue, if the elf was not already in trouble. This will
				// avoid duplicates in the Queue
				if (!this.amInTrouble) {
					this.scenario.elvesInTrouble.add(this);
					this.amInTrouble = true;
				}
				break;
			case AT_SANTAS_DOOR:
				this.amInTrouble = false;
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
