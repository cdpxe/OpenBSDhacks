# What is located in this directory?

This directory contains some OpenBSD extensions that I developed several years ago but found useful or developed just to learn something about the system. Maybe these patches are useful for someone.

## Patches applied to the OpenBSD code

- 2020-06-05: [Redundant Code Removal](https://github.com/openbsd/src/commit/1185c6907bdef13d649720bbb2070fed64418e6e)
- a few older patches, I forgot them :)

## Patches *not* applied to the official OpenBSD code

- [pf_norm.c.patch.txt](https://github.com/cdpxe/OpenBSDhacks/blob/master/patches/pf_norm.c.patch.txt): extends the `pf` scrubber with the capability to clear the IPv4 reserved bit, if set (written in 2011).

- [`psig`](https://github.com/cdpxe/OpenBSDhacks/blob/master/patches/psig_for_openbsd.patch.html): brings the Solaris `psig` tool to OpenBSD (written in 2006).

- [preventing DoS by file descriptor exhaustion](https://github.com/cdpxe/OpenBSDhacks/blob/master/patches/Re:_Denial_of_service_via_FD_exhaustion.txt): some patch to prevent a ressource exhaustion that possibly leads to a DoS when all file descriptors of a host are consumed (written in 2006).

- [safe_finger](https://github.com/cdpxe/OpenBSDhacks/blob/master/patches/safe_finger_features_for_finger_tool.txt): restricted mode for `finger` (written in 2006).

- [Solaris' -X for `tar`](https://github.com/cdpxe/OpenBSDhacks/blob/master/patches/tar_addon.patch.htm): Solaris had the -X parameter for tar (OpenBSD too, but with different functionality, so I made this the -E parameter; written in 2005).

- [tcpdump_print-cdp.c](https://github.com/cdpxe/OpenBSDhacks/blob/master/patches/tcpdump_print-cdp.c.txt): better `tcpdump` output for the CDP (written in 2006)

- [usr/bin/from](https://github.com/cdpxe/OpenBSDhacks/blob/master/patches/usr.bin_from.txt) – some patch for `from` (written in 2006)

