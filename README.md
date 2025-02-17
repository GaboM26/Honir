# Project Honir

*Honir* - In Norse Mythology, Hœnir is a god of the Aesir tribe. He is associated with mystery, wisdom, and silence. he often appears in myths regarding the creation of humanity. 
Being Odin's brother, Hœnir is more than just a secondary or a passing figure in Norse Mythology. He is one of the few surviving Gods after Ragnarok.

Project Honir attempts to provide basic crypthographic tools in an easily modifiable codebase that uses cutting edge security
techniques to utilize in bigger projects. A perfect example of the power of these tools can be seen in cstore code.
Honir was developed as a homework assignment during a Security course during my time in Columbia University. Since then,
modifications/upgrades have and will be realized in the codebase.

## Installation

To install Honir, clone the repository and build the project using the provided Makefile:

```sh
git clone https://github.com/gabo_m26/Honir.git
cd Honir
make
```

## Hmac
Hmac is exactly what its name says. It is a program made to take some input file (or text) and output the hash
of that input. Hashes, by nature, are deterministic and are used in order to securely validate keys. The algorithm 
of choice is SHA-256 and leverages other open source code in order to achieve its functionality.

### Usage
```sh
./hmac --help
```

## AES
AES utilizes cbc in order to ensure difficult to crack encryption software based on an input key. It takes in a 
source and destination files as input, as well as a key to perform encryption. The code provides both an encryption
and decryption program in order to fully benefit from this program. The algorithm of choice here is Advanced
Encryption Standard (AES).

### Usage
```sh
./aes-encrypt --help
```
```sh
./aes-decrypt --help
```
## CStore 

Cstore (cryptography store) is a data very simple, secure data storage program which will encrypt a set of files
into a single file and make its contents hidden. It ensures data is never modified or read by utilizing a combination
of cbc AES and SHA-256. Given a file is modified, the whole store essentially autodestructs itself to prevent any
of the information to continue to be touched.

### Usage 

```sh
./cstore add <archive_name> <file1> <file2> ...
```

```sh
./cstore extract <archive_name> <file>
```

```sh
./cstore list <archive_name>
```


### Technical/Implementation details

Data model for archives is the following: All areas can be of n size. 

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

Metadata and Files are encrypted utilizing aes_cbc encryption/decryption

The preferred method of encryption was cbc in order to prevent
any unintended data leakage from attackers (particularly, that
which ecb mode may bring out).


# Integrity 

Integrity ensured via a SHA-256 HMAC appended at the end of the file.

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

add will create an archive and doesn't add more files to an existing archive.
Furthermore, for extract, all the archive is read into
memory and then moved around from there. Some "still reachable" blocks
were detected coming from cstore_args.cpp code, which is from already existing
scaffolding provided by teaching staff. No other data leaks detected.