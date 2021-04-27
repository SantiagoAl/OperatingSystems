import java.util.Random;


public class Reindeer implements Runnable {

	public enum ReindeerState {AT_BEACH, AT_WARMING_SHED, AT_THE_SLEIGH};
	private ReindeerState state;
	private SantaScenario scenario;
	private Random rand = new Random();
	private boolean exit = false;

	/**
	 * The number associated with the reindeer
	 */
	private int number;

	/**
	 * Santa might call this function to send them to the Sleigh
	 * @param state
	 */
	public void setState(ReindeerState state) {
		this.state = state;
	}
	
	public Reindeer(int number, SantaScenario scenario) {
		this.number = number;
		this.scenario = scenario;
		this.state = ReindeerState.AT_BEACH;
	}

	@Override
	public void run() {
		while(!this.exit) {
		// wait a day
		try {
			Thread.sleep(100);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		// see what we need to do:
		switch(state) {
		case AT_BEACH: { // if it is December, the reindeer might think about returning from the beach
			if (scenario.isDecember) {
				if (rand.nextDouble() < 0.1) {
					state = ReindeerState.AT_WARMING_SHED;
					this.scenario.numReindeerReady++;
				}
			}
			break;			
		}
		case AT_WARMING_SHED:
			// If all of the reindeer are home, the last reindeer will wake up santa. Otherwise,
			// they will wait until the last reindeer shows up
			try {
				if (scenario.numReindeerReady > 8) {
					this.scenario.santa.wakeUpSanta(1);
				}

				this.scenario.waitForLastReindeer.acquire();
			} catch (Exception e) {
				e.printStackTrace();
			}
			break;
		case AT_THE_SLEIGH: 
			// Here, the reindeer are ready at the sleigh
			break;
		}
		}
	};

	public void endThread() {
		this.exit = true;
	}
	
	/**
	 * Report about my state
	 */
	public void report() {
		System.out.println("Reindeer " + number + " : " + state);
	}
	
}
