import java.util.ArrayList;
import java.util.concurrent.Semaphore;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;



public class SantaScenario {

	public Santa santa;
	public List<Elf> elves;
	public Queue<Elf> atSantasDoor;
	public Queue<Elf> elvesInTrouble;
	public Semaphore inTrouble;
	public Semaphore fixingElvesProblems;
	public boolean isDecember;
	
	public static void main(String args[]) {
		SantaScenario scenario = new SantaScenario();
		scenario.inTrouble = new Semaphore(3, true);
		scenario.fixingElvesProblems = new Semaphore(1, true);
		scenario.isDecember = false;
		// create the participants
		// Santa
		scenario.santa = new Santa(scenario);
		Thread th = new Thread(scenario.santa);
		th.start();

		// The elves: in this case: 10
		scenario.elves = new ArrayList<>();

		for (int i = 0; i != 10; i++) {
			Elf elf = new Elf(i+1, scenario);
			scenario.elves.add(elf);
			th = new Thread(elf);
			th.start();
		}

		// Here, we can will create a Queue to hold the elves that are in
		// trouble, as well as the elves that are at Santas Door
		scenario.atSantasDoor = new LinkedList<>();
		scenario.elvesInTrouble = new LinkedList<>();

		// now, start the passing of time
		for (int day = 1; day < 500; day++) {
			// wait a day
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}

			// turn on December
			if (day > (365 - 31)) {
				scenario.isDecember = true;
			}

			if (day == 370) {
				scenario.santa.endThread();

				for (Elf elf : scenario.elves) {
					elf.endThread();
				}
			}

			// NOTE: Here, after 370 days the last state will be printed out
			// until the 500 days
			// print out the state:
			System.out.println("***********  Day " + day + " *************************");
			scenario.santa.report();

			for (Elf elf: scenario.elves) {
				elf.report();
			}

			// Now, we check if there are at least three elves in trouble.
			// If so, we change their state to AT_SANTAS_DOOR. We use another semaphore
			// to make sure we only send three to santas door.
			try {
				if (scenario.elvesInTrouble.size() > 2) {
					if (scenario.atSantasDoor.isEmpty()) {
						scenario.fixingElvesProblems.acquire();
						while (!scenario.elvesInTrouble.isEmpty()) {
							Elf elf = scenario.elvesInTrouble.remove();
							elf.setState(Elf.ElfState.AT_SANTAS_DOOR);
							scenario.atSantasDoor.add(elf);
						}
						scenario.fixingElvesProblems.release();
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
}
