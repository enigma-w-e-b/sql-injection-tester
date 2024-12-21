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

- libcurl
- cmake
- make

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
```

## Licença

Este projeto é distribuído sob a licença MIT. Consulte `LICENSE` para obter mais informações.

# Termos e Responsabilidades

## Introdução

As ferramentas desenvolvidas pela Ciphers Systems são projetadas para ajudar os usuários a aprender sobre criptografia e segurança da informação. Estas ferramentas são fornecidas gratuitamente e devem ser utilizadas de forma responsável e ética.

## Uso Permitido

Os usuários das ferramentas da Ciphers Systems devem utilizar esses recursos apenas para:

1. **Educação:**
    - Aprender sobre criptografia e segurança da informação.
    - Realizar estudos acadêmicos e projetos educacionais.

2. **Desenvolvimento de Habilidades:**
    - Melhorar as habilidades técnicas em criptografia e segurança.
    - Praticar a implementação de algoritmos criptográficos de maneira ética.
    - Aprender a identificar e corrigir vulnerabilidades de segurança.
    - Desenvolver habilidades práticas em testes de penetração.
    - Aprender a proteger sistemas e dados contra ameaças cibernéticas.
    - Aprender a usar ferramentas de segurança e criptografia de forma responsável.
    - Aprender a proteger a privacidade e a segurança dos dados pessoais.

## Responsabilidade do Usuário

1. **Uso Ético:**
    - Os usuários são responsáveis por garantir que suas atividades estejam em conformidade com todas as leis e regulamentos aplicáveis.
    - É estritamente proibido utilizar as ferramentas para qualquer atividade ilegal, incluindo, mas não se limitando a, invasão de sistemas, roubo de dados ou qualquer outra forma de cibercrime.

2. **Consequências do Uso Indevido:**
    - A Ciphers Systems não se responsabiliza por qualquer ato ilegal ou antiético realizado pelos usuários das ferramentas.
    - Os usuários assumem total responsabilidade por suas ações e concordam em indenizar a Ciphers Systems por qualquer dano resultante do uso indevido das ferramentas.

## Isenção de Responsabilidade

1. **Limitação de Responsabilidade:**
    - As ferramentas são fornecidas "como estão", sem garantias de qualquer tipo, expressas ou implícitas.
    - A Ciphers Systems não garante que as ferramentas estejam livres de erros ou falhas, ou que o uso das ferramentas atenderá às expectativas dos usuários.

2. **Isenção de Responsabilidade:**
    - Em nenhuma circunstância a Ciphers Systems será responsável por qualquer dano indireto, incidental, especial, consequente ou punitivo decorrente do uso ou da incapacidade de usar as ferramentas.

## Aceitação dos Termos

Ao utilizar as ferramentas da Ciphers Systems, os usuários concordam com os termos e responsabilidades descritos neste documento. Se você não concorda com qualquer parte destes termos, não utilize as ferramentas.

## Atualizações dos Termos

A Ciphers Systems reserva-se o direito de atualizar estes termos a qualquer momento. Quaisquer alterações serão publicadas nesta página e entrarão em vigor imediatamente após a publicação.

---

Se você tiver alguma dúvida sobre esses termos, por favor, entre em contato conosco através do [site oficial](https://ciphers.systems).

**Última atualização:** 21 De Dezembro De 2024
