#ifndef PROCESADOR_VENTAS_H
#define PROCESADOR_VENTAS_H

#include <string>
#include "../modelos/Venta.h"
#include "Inventario.h"
#include "../datos/ArchivoVentas.h"

namespace pos {

class ProcesadorVentas {
public:
    ProcesadorVentas(Inventario& inventario, ArchivoVentas& archivoVentas);

    bool realizarVenta(const std::string& username,
                       const std::vector<LineaVenta>& lineas,
                       Venta& ventaGenerada);

private:
    Inventario& inventario;
    ArchivoVentas& archivoVentas;

    int64_t generarIdVenta();
};

} // namespace pos

#endif // PROCESADOR_VENTAS_H