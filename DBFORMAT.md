## Possible layout for the binary database format:

**8 bytes header:** "PONIKODB"

**entry:**
 * 8 bytes timestamp
 * 1 byte server name length
 * dynamic size server name
 * 1 byte channel name length
 * dynamic size channel name
 * 1 byte nick size
 * dynamic size nick
 * 2 bytes quote size
 * dynamic size quote

That's all. It will all be a supercompacted blob. Kind of like the original, but even more compacted, and with additional entries. The same format can be used for the "last seen" database as well as the quote db.
