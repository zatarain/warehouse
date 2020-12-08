---
author: zatarain
---
# Warehouse
This project aims to be an exercise to discuss about software engineering technical topics like system design, testing, deployment, etcetera.

## Overview
This simple application aims to manage a warehouse to hold articles, and the articles should contain an identification number, a name and available stock.

The warehouse application will also have to manage products. Products are made of different articles. Products should have a name, price and a list of articles of which they are made from with a quantity.

The data source for both (products and articles) will be text files in JSON format.

## Tenets
The primary values for the implementation will be: simplicity, readability, maintainability, testability. It should be easy to scan the code and rather quickly understand what it’s doing.

### Scope
* Load the data from the JSON files.
* Write the results on the JSON files.
* Validate user input and data in the files.
* Show the output to the user in JSON format.

### Assumptions
* The application is not thread-safe.
* The files are locally stored in specific folder with the specific provided names.
* Since the data is on text files, we won't any implement transactional or ACID model.
* Text files are well formed and has specific format.
* Data in the text files is consistent as described below in Data section.

# Design
Following subsections aim to briefly describe the desing of the solution, description of the data, the interactions with the user and expected behaviours.

## Workflows
The warehouse application will work in following way:
1. Load the data from the files at the start of the application.
2. Sequentially allow the user request following operations:
   * List all products and their availability based on the current inventory.
   * Sell a product and update the correspondent articles.
   * Show the help for the application.
   * Exit from the application.

### Load data
**Input:** None
1. For each of the data files (products and inventory):
   1. Open the file
   2. Parse the JSON file
   3. Strucuture the data for convinient access
2. Make the link/join between products and articles.
3. Compute the initial availability for all the products.

### List all products
**Input:** None

**Steps:**
1. For each product:
   1. Print its name and availability

### Sell a product
**Input:** Product name

**Steps:**
1. Validate the user input.
2. If it's invalid, show an error message and end this workflow.
3. Look for the product on the loaded data.
4. If the product exists:
   1. Check its availability, if it's available:
      1. Display list of articles (names) and amounts
	   2. Update the inventory of articles
	   3. Update the availability only for the impacted producs
   2. Otherwise: tell to the user that product is unavailaible.
5. Otherwise: tell to the user product doesn't exist.

### Show help
**Input:** None

**Steps:**
1. Show brief description of the available operations with their expected input and output.

### Exit
**Input:** None

**Steps:**
1. Write the data on the correspondent files.
2. Close the files properly.
3. Terminate the application.

## Data
### Models
### Input
### Output
### Validations
### Views
## User interaction

## Deployment

## Tests

# Implementation

# Further work