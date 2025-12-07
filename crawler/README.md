# TSE Crawler
## CS50 TSE 25F GROUP C
### Authors:
#### Maryori Maloney (maryorimaloney)
#### Annabelle Hermey (AHermey)
#### Pattapol (Phoom) Sirimangklanurak (LinkZippy)
#### Reed Levinson (reedlevinson)
##

Note: the rest of Crawlers's `README.md` can be found in the `DESIGN.md` and `Implementation.md` files becasue we seperated markdown files following the instructions for Querier for continuity.


The crawler program accepts three arguments: crawler, seedURL, pageDirectory, and maxDepth. The crawler starts from the seedURL and systematically crawls the web by following links, saving each webpage to the pageDirectory. The maxDepth parameter limits how many links away from the seed URL the crawler will traverse. Each saved page is assigned a sequential document ID starting at 1. Additional implementation details are provided in the Crawler Implementation Spec.

## Assumptions

There were no assumptions used beyond the scope of the assignment.

## Testing

For compiling, the command `make` compiles all files for the crawler. For testing, the command `make test` runs testing.sh. For cleaning, the command `make clean` deletes all files created by the make command.

The test suite includes:
- Invalid argument tests (wrong count, invalid URL, invalid directory, invalid depth)
- Valid crawling tests with different depths
- Tests with different seed URLs (letters, toscrape, wikipedia)

## Known Failures

There are no known failures we are aware of with the crawler.
