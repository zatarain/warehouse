# Warehouse help
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
