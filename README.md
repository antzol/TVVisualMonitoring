# TVVisualMonitoring
The application package for visual monitoring of television programs (IPTV, DVB-T2 and etc.).

**The project is under development.**

The project currently consists of
- [tvvm-server](./TVVMServer/README.md) is an application for receiving transport streams and displaying TV programs in the form of a mosaic;
- [tvvm-configurator](./Configurator/README.md) is an application for configuring this application package.

## Additional features

### Processing of audio streams

- Measurement of the level of audio signals according to [recommendations ITU-R BS.1770-4 «Algorithms to measure audio programme loudness and true-peak audio level»](https://www.itu.int/dms_pubrec/itu-r/rec/bs/R-REC-BS.1770-4-201510-I!!PDF-E.pdf).


## Screenshots

20 TV channels from different sources.

![TVVM - Viewer Window](./docs/screenshots/tvvm-server/server%20-%20viewer%20window%2001.png)

Volume indicators 3 radio programs in 1 cell (1 row, 1 column). 2 TV channels are scrambled (line 1, columns 2 and 3).

![Volume level indicators 3 radio programs in 1 cell](./docs/screenshots/tvvm-server/server%20-%20viewer%20window%2002%20-%203%20radio%20in%201%20cell.png)
