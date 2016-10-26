#DENET  
Denet é uma plataforma para análise fundamentalista das companhias de capital aberto cadastradas na CVM (Brasil). 

##Página principal
Mais informações incluindo _screenshots_ e instruções de utilização podem se encontrar [na página principal](http://www.potuz.net/denet). 

## Principais Funcionalidades
Neste momento o programa consta de três partes principais

1. Uma Biblioteca `C++` `libdfp` que nesta versão suporta:
	* Listar arquivos disponíveis na CVM para download.
	* Baixar arquivos automaticamente da CVM.
	* Importar ITR e DFP para uma base de dados MySQL (local ou externa).
	* Reportar indicadores fundamentalistas de uma empresa dada.
	* Criar e iniciar a base de dados no servidor indicado (precisa acesso root ao servidor MySQL). 
2. Ferramentas CLI escritas em `C++` `denet` e `cvmfetch` que são clientes de `libdfp` na linha de comando. 
3. Um GUI `genet` escrito em `C++` utilizando `QT5` que além de implementar todas as funcionalidades de `libdfp` também suporta:
	*Resumo dos indicadores fundamentalistas principais, trimestrais e anualizados das companhias. 
	*Visualização e edição do balanço completo, correção das entradas na base de dados. 
	*Gráficos de evolução dos principais indicadores fundamentalistas
	*Reportes dos resultados das companhias. 

##Requisitos
1. `libdfp` requere de 
	* `MySql`.
	* `MySql Connector++`.
	* [libarchive](http://www.libarchive.org) - Para descomprimir os archivos da CVM.
	* [libcurl](https://curl.haxx.se/libcurl/) - Para baixar os archivos da CVM e obter preços do [Yahoo!](https://finance.yahoo.com/q?s=API) 
	* [pugixml](http://pugixml.org/) - Para ler os documentos `XML` da CVM (incluído nesta distribuição).
2. `denet` e `cvmfetch` precisam além de 
	+ [ezOptionParser](http://ezoptionparser.sourceforge.net) - Para analisar os argumentos no CLI (incluído nesta distribuição). 
3. `genet` precisa de
	+ [Qt5>=5.7](http://qt-project.org/qt5) incluindo `QtWidgets`, `QtConcurrent` e `QtCharts`. 
4. Para compilar precisa de pelo menos um compilador que possa interpretar `-std=c++14`. O sistema de instalação precisa de `CMake>=3.1.0`
5. Antes de correr o programa pela primeira vez certifique que sua instalação de `MySQL` está funcionando e que você tem acesso ao usuário administrador (geralmente _root_).  

##Instalação a partir do código fonte:
###Linux
+ Desde o diretório base (onde `CMakeLists.txt` se encontra)
    mkdir build && cd $_
    cmake ../
    make
    sudo make install

###Windows
+ Abra o GUI de `CMake`
+ Escolha o diretório onde você descompactou as fontes e o diretório onde vai compilar (pode ser o mesmo das fontes, mas preferivelmente um novo diretório por exemplo `build`). 
+ Clique em `Configure`
+ Abra o projeto em visual studio e compile 

##Autor 
**Potuz** <potuz@potuz.net>

##Contato
Normalmente estou em `#denet` [@freenode](http://webchat.freenode.net), se não estou pode deixar uma mensagem lá ou por e-mail. 

Também estou regularmente no [ClubInvest](http://clubinvest.boards.net). 

##Licencia 
O projeto está distribuído sob a proteção do Gnu GPL v3. Veja o documento [COPYING](COPYING) para mais informação. 

##Agradecimentos
+ _rufuslenhador_ que ajudou em versões prévias com a base de dados `SQLite`
+ Ao pessoal do [ClubeInvest](http://clubinvest.boards.net)
