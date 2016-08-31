# fuzzy k-means
./fkm -k 4 -m 2 -d 1 -i 200 -if ../data/CO.txt -pf p-CO-fkm.csv -ef e-CO-fkm.csv
./fkm -k 4 -m 2 -d 1 -i 200 -if ../data/NO2.txt -pf p-NO2fkm.csv -ef e-NO2fkm.csv
./fkm -k 4 -m 2 -d 1 -i 200 -if ../data/O3.txt -pf p-O3fkm.csv -ef e-O3fkm.csv
./fkm -k 4 -m 2 -d 1 -i 200 -if ../data/SO2.txt -pf p-SO2fkm.csv -ef e-SO2fkm.csv
./fkm -k 4 -m 2 -d 4 -i 200 -if ../data/AirQualityNY-2013-2015.txt -pf pertinence-fkm.csv -ef edges-fkm.csv
# k-means
./fkm -k 4 -m 1.001 -d 1 -i 200 -if ../data/CO.txt -pf p-CO-km.csv -ef e-CO-km.csv
./fkm -k 4 -m 1.001 -d 1 -i 200 -if ../data/NO2.txt -pf p-NO2km.csv -ef e-NO2km.csv
./fkm -k 4 -m 1.001 -d 1 -i 200 -if ../data/O3.txt -pf p-O3km.csv -ef e-O3km.csv
./fkm -k 4 -m 1.001 -d 1 -i 200 -if ../data/SO2.txt -pf p-SO2km.csv -ef e-SO2km.csv
./fkm -k 4 -m 1.001 -d 4 -i 200 -if ../data/AirQualityNY-2013-2015.txt -pf pertinence-km.csv -ef edges-km.csv
#
