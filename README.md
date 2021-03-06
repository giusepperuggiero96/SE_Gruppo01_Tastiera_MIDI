# Progetto P1 Sistemi Embedded
## Gruppo 01: Barletta, Corvi, Marchetta, Ruggiero

<P align="center">
<img src="https://github.com/giusepperuggiero96/SE_Gruppo01_Tastiera_MIDI/blob/71d71191e0dc0e992c8aea036889fa53c0cd0719/imgs/photo_2021-06-14_12-36-59.jpg" width="450">
</p>

## Sviluppo di una tastiera MIDI su breadboard
La documentazione completa del progetto è disponibile sulla [Wiki](http://www.naplespu.com/es/index.php?title=Sviluppo_di_un%27applicazione_basata_su_middleware_STM32Cube:_USB_DeviceAudio_Class).</br>
Una demo del funzionamento della tastiera è disponibile in questo [Video](https://youtu.be/TmSivjw7Sj4).

## Compilazione
Per compilare l'applicazione basta spostarsi da terminale nella cartella `deploy` e lanciare in sequenza i comandi:
```
$ make prepare
$ make
```
ed in seguito per lanciare l'applicazione aprire openOCD oppure qualunque altro on-chip debugger. Nel caso si faccia uso del debugger `gdb-multiarch` lanciare il comando:
```
$ make debug
```
Nel caso invece si faccia uso del debugger `arm-none-eabi-gdb` lanciare invece il comando:
```
$ make debug2
```
A questo punto aprendo una qualunque Digital Audio Workstation (ad esempio [lmms](https://lmms.io/lsp/), workstation FOSS per linux) si può testare il funzionamento del programma premendo il bottone *user* che riprodurrà un brano di esempio, oppure, nel caso si disponga della breadboard si può suonare premendo i bottoni e modificare il pitch girando la rotellina del potenziometro.
