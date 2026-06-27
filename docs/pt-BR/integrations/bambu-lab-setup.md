# Configuração de Bambu Lab para iHeater Link

## Para que isso é necessário

iHeater Link pode gerenciar automaticamente o iHeater durante a impressão em impressoras Bambu Lab. Diferentemente do cenário Klipper, não é necessário adicionar macros ou modificar o G-code na impressora Bambu. Link se conecta à impressora pela rede local e lê o estado de impressão e o filamento ativo.

Nas impressoras Bambu Lab atuais, não é possível usar de forma confiável a transmissão de temperatura da câmara como comando de controle do iHeater. No entanto, Link pode determinar qual filamento está instalado no servidor ativo e se a impressora iniciou a preparação ou já está imprimindo. Com base no tipo de filamento, o firmware seleciona a temperatura de câmara necessária da tabela de materiais e liga automaticamente o iHeater.

Se o tipo de filamento necessário não estiver na lista de materiais do firmware, entre em contato com o autor do firmware: o tipo pode ser adicionado em versões futuras.

## O que será alcançado

```text
Bambu inicia a preparação ou impressão -> Link vê o filamento ativo -> seleciona a temperatura da tabela de materiais -> iHeater é ligado
```

Quando a impressão termina ou o cenário ativo não requer mais aquecimento, Link desliga o iHeater.

## 1. Abra as configurações do dispositivo

No portal, abra o cartão iHeater Link e clique no ícone de engrenagem.

![Abertura das configurações do dispositivo](../../img/iheater-link-settings-bambu-02.png)

## 2. Ative a conexão Bambu

Nas configurações do dispositivo, abra o bloco **CONNECTIONS** e ative **BAMBU**. As outras conexões podem ser deixadas desativadas se não forem usadas.

![Ativação de Bambu nas configurações do dispositivo](../../img/iheater-link-settings-bambu-03.png)

## 3. Selecione Bambu Lab na página do dispositivo

Retorne à página do dispositivo e clique em **BAMBU LAB** no bloco **Device Info**. O botão se tornará ativo.

![Seleção de Bambu Lab](../../img/iheater-link-settings-bambu-04.png)

## 4. Insira os parâmetros de conexão

Nas configurações de **Bambu Lab**, ative a integração e preencha os parâmetros de conexão.

![Configurações de Bambu Lab](../../img/iheater-link-settings-bambu-05.png)

Normalmente, os seguintes campos são necessários:

- Printer IP: endereço IP da impressora na rede local;
- Printer serial: número de série da impressora;
- LAN access code: código de acesso do modo LAN;
- Auto-apply on tag detect: ativado se Link deve aplicar automaticamente a temperatura baseado no filamento reconhecido;
- Default AMS e Default tray: você pode deixar os valores padrão se não precisar forçar a seleção de um AMS ou servidor específico.

A impressora e o iHeater Link devem estar na mesma rede local. O código de acesso LAN e o número de série são obtidos nas configurações da impressora Bambu Lab.

## 5. Configure as temperaturas dos materiais

Abra o bloco **MATERIALS** nas configurações do dispositivo. Para cada tipo de filamento, você pode definir sua própria temperatura de câmara.

![Temperaturas dos materiais](../../img/iheater-link-settings-001-materials.png)

O firmware já possui um amplo conjunto de tipos de filamentos. Quando a impressora iniciar a preparação ou impressão, Link verificará o servidor ativo, determinará o tipo de filamento e usará a temperatura desta tabela. Por exemplo, para PLA você pode deixar uma temperatura baixa ou desativar o aquecimento, para ABS e ASA definir uma temperatura mais alta.

Se o filamento ativo não for encontrado na tabela ou se a temperatura definida não for apropriada, edite o valor em **MATERIALS** e salve as configurações.

## 6. Verifique o funcionamento

Inicie uma impressão na Bambu Lab com um filamento para o qual a temperatura foi definida em **MATERIALS**. Quando a impressora passar para a preparação ou impressão, iHeater Link deve aplicar automaticamente a temperatura do filamento ativo e ligar o iHeater.

Se o aquecimento não é ativado, verifique:

- se a conexão **BAMBU** está ativada nas configurações do dispositivo;
- se **BAMBU LAB** está selecionado no bloco **Device Info**;
- se IP, serial e código de acesso LAN estão corretos;
- se a impressora vê o servidor ativo e o tipo de filamento;
- se a temperatura foi definida para este tipo de filamento em **MATERIALS**.
