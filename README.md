# LoRa_CTC_CPTM
Códigos criados para execução do projeto de monitoramento dos CTCs

Esse projeto tem como objetivo diminuir o impacto que as falhas de um sistema vital de
alimentação do sistema de sinalização tem
na operação dos trens da CPTM e por consequência nos usuários.

Ele consiste em fazer o monitoramento da linha de alimentação principal denominada CTC 1 e
sua redundância chamada CTC 2.

Usamos para o protótipo inicial dois dispositivos LoRa para comunicação entre si devido as
grandes distâncias, ou seja, 
um emissor e um receptor.

O emissor faz a leitura dos pinos 34 (Sinal do CTC1 - Nível lógico alto) e  35 (Sinal do 
CTC2 - Nível lógico alto)
que representa uma condição normal, caso algum deles venha a ficar em nível lógico baixo o
mesmo estará em falha.
Com estas informações o emissor enviará de acordo com cada condição um pacote com dados por
meio da comunicação LoRa (Long Range) para o receptor.

Já o receptor ao receber o pacote com os dados os analisa e de acordo com as condições
aciona a saída 12 (Referente a falha no CTC 1) ou a saída 13 (Referente a falha no CTC 2)
ou ambos na qual representa falha nos dois CTCs e PANE na sinalização no trecho monitorado.

Ao mesmo tempo há comunicação via Wi-fi com o supervisório ScadaBR via protocolo ModbusIP 
ao qual apresenta de modo gráfico as falhas no trecho monitorado.

Este repositório contem a programação utilizada para:

Emissor

Receptor
