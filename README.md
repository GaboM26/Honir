[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/xGB-fK-g)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-718a45dd9cf7e7f842a935f5ebbe5719a5e09af4491e668f4dbf3b35d5cca122.svg)](https://classroom.github.com/online_ide?assignment_repo_id=12097733&assignment_repo_type=AssignmentRepo)
# HW1
 
## Reminder:

* Please remember to place your UNI in `UNI.txt`. Do not put anything else in this file

* Please remember to comlete `references.txt`
 
## Instructions:

Please refer to the assignment pdf prompt released in Courseworks for the specification in this homework assignment. 

Replace this README to document edge cases, testing, or deviations from the specification.

Further, put your documentation for Part3 in this file.

## Part 3 Documentation

For Cstore, the 3 functions are supported. The data model used for
the archives are the following. All areas can be of n size. 

METADATA: N bytes
holds information such as number of records, titles of documents
within archive, and [...] . 

SEPARATOR: AES_BLOCK_SIZE of null bytes.

IV: for every file, the respective IV.
FILES: Encrypted file follows
SEPARATOR: AES_BLOCK_SIZE of null bytes.

MAC: HMAC of complete file at the end (32 bytes)

# Encryption:

Metadata and Files are encrypted utilizing aes_cbc encryption/decryption (utility developed in part2).

The preferred method of encryption was cbc in order to prevent
any unintended data leakage from attackers (particularly, that
which ecb mode may bring out).


# Integrity 

Integrity ensured via a MAC appended at the end of the file.
HMAC utilized is that developed in part 1. In other words,
hash function used for HMAC is SHA-256.

Extra sanity checks, such as that the number of files listed in metadata matches with the number of files found, are utilized to add a small extra layer to integrity.

# Keys:

All files are protected utilizing input password.

Metadata also encrypted for added security, even though information kept here is technically not too important.
Key for encryption is simply archive name. This choice
was made to protect against the "less smart" attackers
that may want to take a peak at the contents of the 
archive. This makes all the metadata intelligible from
these attackers. For the smartest attacker, even if
they were to "crack the code", no vital information would be 
leaked, since it only contains data that can be received
by running ./cstore list.