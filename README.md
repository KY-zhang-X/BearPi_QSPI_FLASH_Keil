# BearPi_QSPI_FLASH_Keil
A flash download algorithm for bearpi iot board

## Usage
1. Copy BearPi_W25Q64JV.FLM into Keil flash algorithm directory(e.g. `E:/Keil/ARM/Flash`)

2. Select BearPi_W25Q64JV.FLM in Keil IDE.
    * Go to **Project - Options for Target - Debug** tab.
    * In the **Use** row select from the drop-down menu the target debug adapter. Press **Settings** button.
    * Go to the **Flash Download** tab.
    * **Press** the Add button. Find and select the target flash programming algorithm with description **"BearPi W25Q64JV 8MB Flash"**.
    * Press **Add**. The algorithm appears in the list of programming algorithms to be used by the debug adapter.
    * Verify the values specified in the **RAM for Algorithm** section. Especially the Size value needs to be large enough to ensure there is sufficient space available for the algorithm(e.g. 0x00010000).