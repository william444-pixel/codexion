*This project has been created as part of the 42 curriculum by nael-oua.*

## Description
Codexion is a multithreaded simulation project that models the complexities of concurrent resource sharing. In a circular co-working hub, coders alternate between compiling, debugging, and refactoring. To compile their quantum code, each coder must simultaneously acquire two shared USB dongles (one on their left, one on their right). The simulation's objective is to orchestrate these coders efficiently using POSIX threads, preventing deadlocks, data races, and ensuring no coder starves to the point of burnout.

## Instructions
**Compilation:**
Run the `make` command at the root of the repository to compile the program. This will generate the `codexion` executable.
- `make clean`: Removes object files.
- `make fclean`: Removes object files and the executable.
- `make re`: Recompiles the entire project.

**Execution:**
Run the program with the following mandatory arguments:
`./codexion [number_of_coders] [time_to_burnout] [time_to_compile] [time_to_debug] [time_to_refactor] [number_of_compiles_required] [dongle_cooldown] [scheduler]`

**Example:**
`./codexion 5 3000 200 200 200 10 400 fifo`
- **Scheduler:** Must be either `fifo` (First In, First Out) or `edf` (Earliest Deadline First).

## Resources
- **POSIX Threads Documentation:** `man pthread_create`, `man pthread_mutex_lock`.
- **Valgrind (Helgrind & DRD):** Used extensively for detecting race conditions and deadlocks in multithreaded execution.
- **AI Usage:** 

## Blocking cases handled
- **Deadlock Prevention (Coffman's Conditions):** To prevent circular wait deadlocks, even-numbered and odd-numbered coders request their dongles in a specific order (e.g., the last coder attempts to take the right dongle before the left, breaking the symmetry).
- **Starvation Prevention & Cooldown:** A strict Timestamp Ordering strategy ensures fair access. Coders register their priority keys (timestamps or deadlines) simultaneously in both dongle priority queues. The `dongle_cooldown` mechanism restricts immediate re-acquisition, forcing fast coders to wait and allowing slower or starved coders to proceed.
- **Precise Burnout Detection:** A dedicated monitor thread continuously checks all coders. Because coders use a custom micro-sleeping function (`custom_sleep`) that polls the state incrementally instead of blocking on long `usleep` calls, the monitor can flag a burnout and halt the simulation securely within the required 10ms tolerance.
- **Log Serialization:** A dedicated `print_lock` mutex wraps all terminal output. This guarantees that `printf` messages never interleave or write over each other, even under heavy contention.

## Thread synchronization mechanisms
- **Mutexes (`pthread_mutex_t`):** 
  - Each dongle is protected by its own mutex (`dongle->lock`) to prevent simultaneous access or modification of its state and priority queue.
  - A global simulation mutex (`sim_lock`) protects shared readable/writable states such as `stop_flag`, `compiles_done`, and timestamps.
  - A dedicated printing mutex (`print_lock`) synchronizes terminal output.
- **Custom Priority Queue (Min-Heap):** To satisfy the requirement for deterministic scheduling without standard library functions, a custom Min-Heap data structure orchestrates the waiting line for each dongle. Depending on the `scheduler` argument, the heap sorts requests either by arrival time (FIFO) or nearest burnout deadline (EDF).
- **Polling vs. Conditional Variables:** Instead of strictly relying on `pthread_cond_t`, coders safely evaluate availability using a thread-safe polling loop combined with `usleep(500)`. If a dongle is unavailable, the coder unlocks the dongle's mutex, sleeps briefly, and re-locks it to check again. This guarantees that the lock is yielded to other coders and avoids lock-ordering deadlocks.