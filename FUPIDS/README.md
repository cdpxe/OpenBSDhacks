# FUPIDS - A "fuzzy" user-profile IDS for OpenBSD

FUPIDS stands for *Fuzzy Userprofile Intrusion Detection System* (it is *not* based on fuzzy logic!). FUPIDS is a kernelcode patch for OpenBSD systems that creates a profile for every user and alerts the admin if an attacker is detected. For FUPIDS, an attacker is a local user who overtakes the account of a user. I wrote the FUPIDS code back in Nov 2003.

The project was heavily discussed and received lots of criticism (mostly due to the bad implementation that I wrote two years before I started studying computer science). Please thus consider FUPIDS as a simple PoC code.

- <s>You can find the announcement mail here.</s> The first public version was released with this mail on Tue, 11 Nov 2003 18:44:57 +0100.
- [deadly.org article (now undeadly.org)](http://undeadly.org/cgi?action=article&sid=20031209154357)
- [slashdot.org article](http://bsd.slashdot.org/bsd/03/12/10/0135257.shtml)
 
## Features
 
Here is a list of FUPIDS' features:

- FUPIDS calculates an attacker level for every user (with uid >= 1000) on your system. It will alert you via syslog if an attacker level becomes too high.
- FUPIDS has a profile of used executables for every user. If a user uses too many new executables in a short time, the attacker level will raise. This is needed, because an attacker could overtake the account of a user and will probably use some new compiled exploits or another editor the normal user never starts.
- FUPIDS reports if your network interfaces (not pflog0 and lo[01]) are going in promiscuous mode (this is linked to the attacker level as well).
- FUPIDS monitors the listen() syscall and will tell you if a user creates a new listen socket (maybe a backdoor).
- If a user who never did anything "bad" before (for example 'uucp') is now active on your system, FUPIDS will notice and report it.
- An attacker cannot kill the FUPIDS system because it is kernel code. The attacker can also not unload a LKM because the code is directly implemented in the kernel.
- Your users do not know that FUPIDS is running on the system, i.e., FUPIDS is transparent.

## Documentation
 
You can find a README file in the .tgz archive and you can find the official documentation I wrote for this project in the list of my publications (**TODO**).

## Download
 
You can download FUPIDS 0.0.4 from GitHub. Just follow the introductions of the 'INSTALL' file to install it. I developed this code for OpenBSD 3.3. However, it should be possible to modify it for the current kernel versions.
