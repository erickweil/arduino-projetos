pub type Result<T> = std::result::Result<T, Box<dyn std::error::Error>>;

/// Macro para encapsular o boilerplate para rodar o código apenas no aparelho
/// e ainda poder ter testes de código puro que rodam no host. 
/// 
/// A ideia é incluir dentro deste macro apenas código específico que depende
/// de recursos do ambiente do ESP-IDF, como acesso a GPIO, Wi-Fi, etc.
/// 
/// Exemplo:
/// ```
/// espidf_std::espidf_only! {
///     pub fn main() -> Result<(), Box<dyn std::error::Error>> {
///         // código do exemplo aqui
///         Ok(())
///     }
/// }
/// ```
#[macro_export]
macro_rules! espidf_only {
    ($($body:tt)*) => {
        #[cfg(feature = "espidf")]
        fn main() -> $crate::Result<()> {
            example::main()
        }

        #[cfg(not(feature = "espidf"))]
        fn main() {
            panic!("Deveria rodar apenas na placa e não no host! Verifique se configurou para usar a feature da placa correta");
        }

        #[cfg(feature = "espidf")]
        mod example {
            use super::*;

            $($body)*
        }
    };
}

pub mod prelude {
    pub use crate::Result;
    pub use crate::espidf_only;
}

// TODO: pensar numa estrutura de Board support package (BSP) futuramente
// pub mod boards;