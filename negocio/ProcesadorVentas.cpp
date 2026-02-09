#include "ProcesadorVentas.h"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace pos {

ProcesadorVentas::ProcesadorVentas(Inventario& inventario,
                                   ArchivoVentas& archivoVentas)
    : inventario(inventario),
      archivoVentas(archivoVentas) {}

int64_t ProcesadorVentas::generarIdVenta() {
    return static_cast<int64_t>(std::time(nullptr));
}

bool ProcesadorVentas::realizarVenta(const std::string& username,
                                     const std::vector<LineaVenta>& lineas,
                                     Venta& ventaGenerada) {
    for (const auto& l : lineas) {
        Producto* p = inventario.buscarPorId(l.productoId);
        if (!p) return false;
        if (p->cantidadInventario < l.cantidad) return false;
    }

    for (const auto& l : lineas) {
        Producto* p = inventario.buscarPorId(l.productoId);
        p->cantidadInventario -= l.cantidad;
    }

    Venta v{};
    v.id = generarIdVenta();
    v.usuario = username;
    v.lineas = lineas;
    v.total = 0.0;

    for (const auto& l : lineas) {
        v.total += l.subtotal;
    }

    std::time_t t = std::time(nullptr);
    std::tm tm{};
	
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    tm = *std::localtime(&t);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    v.fechaHora = oss.str();

    if (!archivoVentas.guardarVenta(v)) {
        return false;
    }
    if (!inventario.guardar()) {
        return false;
    }

    ventaGenerada = v;
    return true;
}

} // namespace pos