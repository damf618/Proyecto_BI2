#include "sapi.h"
#include "gpio.h"
#include "spi_prim.h"

void SPI_INIT(spi_Server_t * pServer1, spiMap_t spi, gpioMap_t pin){
	pServer1->spi=spi;
	pServer1->pin=pin;
	GPIOOutConfig(pServer1->pin);
	GPIOWrite(pServer1->pin,HIGH_G);
	spiConfig( pServer1->spi );
}

bool_t SPI_Server(spi_Server_t * pServer1, uint8_t data)
{
	//charge data
	uint8_t data_load =55;
	//TODO NO SE ENVIA EL 2DODATO NUNCA...
	uint8_t * WriteC[2] = {(uint8_t)WRITE_COMM,0xFF};
	//uint8_t aux=0;
	//gpio off
	GPIOWrite(pServer1->pin,LOW_G);
	spiWrite( pServer1->spi ,WriteC,2);

	//aux=spiRead( SPI0 , aux,1);

	//TODO verificar aux
	//Encaso de Error podria enviar a traves de un Queue una variable para
	//accionar algo


	//gpio on
	GPIOWrite(pServer1->pin,HIGH_G);
	return true;
}
