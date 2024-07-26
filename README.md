# AMR - Automatic Modulation Recognition for wireless RF signals

# RadioModTx
Standalone mode application for transmitting signals from following datasets:
- **DeepSig RadioML 2016.10A**
	- **11** modulations (8 digital and 3 analog)
	- **20** equidistant SNRs (-20:2:18 dB)
	- **1000** frames per each modulation-SNR combination
	- frame length of 128 (I,Q)
	- total number of frames is **220000**
	- pickle file format (*.pkl)
- **DeepSig RadioML 2018.01**
	- **24** modulations (19 digital and 5 analog)
	- **26** equidistant SNRs (-20:2:30 dB)
	- **4096** frames per each modulation-SNR combination
	- frame length of 1024 (I,Q)
	- total number of frames is **2555904**
	- HDF5 file format (*.hdf5, *.h5)
- **HisarMod 2019.1**
	- **26** modulations (20 digital and 6 analog)
	- **20** equidistant SNRs (-20:2:18 dB)
	- **500** frames per each modulation-SNR combination
	- frame length of 1024 (I,Q)
	- total number of frames is **260000**
	- CSV file format (*.csv)

Supported Analog Devices transceivers and MxFEs are:
- [AD9361](https://www.analog.com/en/products/ad9361.html), [AD9363](https://www.analog.com/en/products/ad9363.html), [AD9364](https://www.analog.com/en/products/ad9364.html)
- [ADRV9009](https://www.analog.com/en/products/adrv9009.html)
- [AD9081](https://www.analog.com/en/products/ad9081.html), [AD9082](https://www.analog.com/en/products/ad9082.html)
