# ProofVerifier

Simple project to verify mathematical proofs written in a small custom language.

A first failed attempt, MetaMath, was started on June 1, 2018.
The second attempt was this project, started on July 27, 2018.

Both projects were based on output generated from the Parser project, which was started on May 16, 2018.

The C++ code contained enough functionality to parse `Test.txt` and verify a proof that `2 + 2 = 4`, but the verification code was later lost during several rewrites.

During the project I also explored proofs such as `n * 0 = 0` and `0 * n = 0`, but the code had to be rewritten several times. I was not able to make a small, elegant, and powerful grammar for a proof system.

This project has been terminated. Lean is now the relevant standard for this kind of work.