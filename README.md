# Project Honir

*Honir* - In Norse Mythology, Hœnir is a god of the Aesir tribe. He is associated with mystery, wisdom, and silence. he often appears in myths regarding the creation of humanity. 
Being Odin's brother, Hœnir is more than just a secondary or a passing figure in Norse Mythology. He is one of the few surviving Gods after Ragnarok.
 
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

PASSWORD: 10001 hash of password stored (32 bytes). used
as a further check of integrity.

MAC: HMAC of complete file at the end (32 bytes)

ARCHIVE SIGNIFIER: '&' applied to the end (1 byte). Serves
as a flag to show this is an archive (prevents 99.9% of 
core dumps in a very simple manner).

# Encryption:

Metadata and Files are encrypted utilizing aes_cbc encryption/decryption (utility developed in part2).

The preferred method of encryption was cbc in order to prevent
any unintended data leakage from attackers (particularly, that
which ecb mode may bring out).


# Integrity 

Integrity ensured via a HMAC appended at the end of the file.
HMAC utilized is that developed in part 1. In other words,
hash function used for HMAC is SHA-256.

Extra sanity checks, such as that the number of files listed
in metadata matches with the number of files found, are 
utilized to add a small extra layer to integrity.

Furthermore, 10001 hash of password is stored at the end of all other files. 
10001 chosen because 10000 hash is used as a key. If only
one hash is taken, attacker could in theory do 9999 more hashes
and get access to data. Getting 10001 hash ensures this will 
never happen.

# Bug prevention

Attached '&' at end of file is a very easy way to verify a
file is an archive. Although, in theory, a file could happen to
finish with a '&', it is highly unlikely its the case. 
If most files were randomly made (that they are not), they
would have a 1/256 chance of ending with a '&'. However, most
textfiles, codefiles, and other typical files don't finish
with an odd character such as &, which makes this a simple, yet
highly effective solution to prevent core dumped errors.

# Keys:

All files are protected utilizing input password.

Metadata also encrypted for added security, even though 
information kept here is technically not too important.
Key for encryption is simply archive name. This choice
was made to protect against the "less smart" attackers
that may want to take a peak at the contents of the 
archive. This makes all the metadata intelligible from
these attackers. For the smartest attacker, even if
they were to "crack the code", no vital information would be 
leaked, since it only contains data that can be received
by running ./cstore list.

# other implementation details

add will create an archive and doesn't add more files to an existing archive
just as lab spec says. Furthermore, for extract, all the archive is read into
memory and then moved around from there. Some "still reachable" blocks
were detected coming from cstore_args.cpp code, which is all the scaffolding
provided by teaching staff. No other data leaks detected.