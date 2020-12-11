# 1. Warehouse
This project aims to be an exercise to discuss about software engineering technical topics like system design, testing, deployment, etcetera.

## 1.1 Overview
This simple application aims to manage a warehouse to hold articles, and the articles should contain an identification number, a name and available stock.

The warehouse application will also have to manage products. Products are made of different articles. Products should have a name, price and a list of articles of which they are made from with a quantity.

The data source for both (products and articles) will be text files in JSON format.

## 1.2 Tenets
The primary values for the implementation will be: simplicity, readability, maintainability, testability. It should be easy to scan the code and rather quickly understand what it’s doing.

### 1.2.1 Scope
* Load the data from the JSON files.
* Write the results on the JSON files.
* Validate user input and data in the files.
* Show the output to the user in format `<Product Name>: <availability>`.

### 1.2.2 Assumptions
* The application is not thread-safe.
* The files are locally stored in specific folder with the specific provided names.
* Since the data is on text files, we won't any implement transactional or ACID model.
* Text files are well formed and has specific format.
* Data in the text files is consistent as described below in **Data** subsection.
* Encoding for the files will be assumed as UTF8.

# 2. Design
Following subsections aim to briefly describe the desing of the solution, description of the data, the interactions with the user and expected behaviours.

## 2.1 Workflows
The warehouse application will work in following way:
1. Load the data from the files at the start of the application.
2. Sequentially allow the user request following operations:
    * List all products and their availability based on the current inventory.
    * Sell a product and update the correspondent articles.
    * Show the help for the application.
    * Exit from the application.

### 2.1.1 Load data
**Input:** None
1. For each of the data files (products and inventory):
    1. Open the file
    2. Parse the JSON file
    3. Strucuture the data for convinient access
2. Make the link/join between products and articles.
3. Compute the initial availability for all the products.

### 2.1.2 List all products
**Input:** None

**Steps:**
1. For each product:
   1. Print its name and availability

### 2.1.3 Sell a product
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

### 2.1.4 Show help
**Input:** None

**Steps:**
1. Show brief description of the available operations with their expected input and output.

### 2.1.5 Exit
**Input:** None

**Steps:**
1. Write the data on the correspondent files.
2. Close the files properly.
3. Terminate the application.

## 2.2 Data
Taking following JSON files as examples, we can see that all the entries in their are either strings, list or objects:

**inventory.json:**
```json
{
  "inventory": [
    {
      "art_id": "1",
      "name": "leg",
      "stock": "12"
    },
    {
      "art_id": "2",
      "name": "screw",
      "stock": "17"
    }
  ]
}
```

**products.json:**
```json
{
  "products": [
    {
      "name": "Dinning Chair",
      "contain_articles": [
        {
          "art_id": "1",
          "amount_of": "4"
        },
        {
          "art_id": "2",
          "amount_of": "8"
        }
      ]
    }
  ]
}
```

So, we need to parse the data to a convinient structure in memory, for instance: numbers to a proper integral data type. We will call those structures: models.

### 2.2.1 Models
Models are a collection of labeled fields and will read and parse the data from the files and put it in a memory dataset which will be a collection of records indexed by a *primary key*. The models will be also able to write the data in the files. We will use following structure and data types to model the data on the files:

From file *inventory.json* each **article** will have following fields:
|Field name |Data type| JSON key |
|:---       |  :---:  | :---     |
|*id*       | integer | `art_id` |
|name       | string  | `name`   |
|stock      | integer | `stock`  |

From file *products.json* each **product** will have following fields:
|Field name   |Data type| JSON key          |
|:---         |  :---:  | :---              |
|*name*       | string  | `name`            |
|requirements |   map   | `contain_articles`|

The mapping for the articles will be a collection of pairs from integer (`art_id`) to integer (`amount_of`).

#### 2.2.1.1 Consistency assumptions
* Each file has a main entry (`inventory` or `products`) which is a list.
* Article IDs are unique and won't change.
* All the integers on the file are actually strings.
* Product names are unique, case sensitive and won't change.
* The articles are not duplicated in the same product.
* All the integers on the file are actually strings.

#### 2.2.1.2 Data relationships
We can see from the sample files that the relationship between products and articles is a many-to-many relationship (`N:M`). So, this implies that we need to take care of both when we update any instance of any entity in order to keep the data integrity.

![Class diagram for data models][data-models]

### 2.2.2 User interaction
This application is designed as a back-end command line interface, once is started it receives request of following types by standard input:

* `list`: Shows the list of products.
* `sell <Product Name>`: Sells a product if exists and is available.
* `help`: Displays this information.
* `exit`: Terminates the application writing inventory file before.
* Otherwise: shows an error message.

### 2.2.2.1 Input
The only request type which receives an input is `sell`, this is the product name, for instance:
```
sell Dinning Chair
```

### 2.2.2.2 Validations
Following validations are applied:
* Check whether the product name exists.
* Check whether the product is available.
* Check whether a requirement (article) exists.

### 2.2.2.3 Output
Following is expected to get in the standard output:
* The `list` request shows the output to the user in format `<Product Name>: <availability>`.
* A prompt message.
* Error message in case of:
    - The request of the user is not recognized.
    - An article was not found.
    - A product was not found.
    - A product is not available.

## 2.3 Deployment
Docker container were used in order to deploy the application. So, once this repositorio is downloaded, the application can be deployed using:

```
docker image build -t warehouse:dev .
```

And it can be run by using:

```
docker run -i warehouse:dev
```

## 2.4 Tests
In order to add some unit testing [utz][utz-library] has been used in the deployment. It's a library which stills in development by myself, but for the purpose of the excersice of show how this can be tested, I think should be enough. The test cases implemented are in the folder utz.

# 3. Implementation
The implementation is written in C++17 and relies on a JSON parsing library called [RapidJSON][rapid-json].

Using M(V?)C design patter (so wi missed the view layer so far), the JSON files are parsed by the models (article and product) using the library and then warehouse controller uses those models.

# 4. Further work
* Add more test cases.
* Add more documentation.
* Add more validations or controls, for instance:
    - check whether the user inputs an empty string as product name.
    - trim the string of the product name input by user.
    - integrity and consitency checks.
* Make the application thread-safe.
* Display the output in several formats, for instance implementing different views.
* Support for different character sets not only UTF8.

[data-models]: docs/img/data-models.png
[utz-library]: https://github.com/zatarain/utz
[rapid-json]: https://github.com/Tencent/rapidjson/releases/tag/v1.1.0