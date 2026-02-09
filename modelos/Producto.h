#ifndef PRODUCTO_H
#define PRODUCTO_H

#include <string>
#include <cstdint>

namespace pos {

enum class TipoUnidad {
    Pieza,
	Kilogramo,
	Litro,
    Granel	
};

enum class TipoPresentacion {
    Ninguna,
    Caja,
    Bolsa
};

struct Producto {
    int64_t id;
    std::string codigoBarras;
    std::string codigoLocal;
    std::string nombre;
    std::string descripcion;

    double precioPublico;
    double precioMayorista;

    TipoUnidad tipoUnidad;
    double cantidadInventario;

    int anioCaducidad;
    int mesCaducidad;
    int diaCaducidad;

    TipoPresentacion presentacion;
    int piezasPorPresentacion;
};

} // namespace pos

#endif // PRODUCTO_H