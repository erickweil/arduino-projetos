pub type Result<T> = std::result::Result<T, Box<dyn std::error::Error>>;

#[macro_export]
macro_rules! target_only {
    ($feature:literal, $($body:tt)*) => {
        #[cfg(feature = $feature)]
        fn main() -> $crate::Result<()> {
            example::main()
        }

        #[cfg(not(feature = $feature))]
        fn main() {
            panic!("Deveria rodar apenas na placa e não no host! Verifique se configurou para usar a feature da placa correta");
        }

        #[cfg(feature = $feature)]
        mod example {
            use super::*;

            $($body)*
        }
    };
}

/// Macro para encapsular o boilerplate para rodar o código apenas no aparelho
/// e ainda poder ter testes de código puro que rodam no host.
///
/// A ideia é incluir dentro deste macro apenas código específico que depende
/// de recursos do ambiente do ESP-IDF, como acesso a GPIO, Wi-Fi, etc.
/// ```
/// espidf_std::espidf_only! {
///     pub fn main() -> Result<()> {
///         // código do exemplo aqui
///         Ok(())
///     }
/// }
/// ```
#[macro_export]
macro_rules! espidf_only {
    ($($body:tt)*) => {
        $crate::target_only!("espidf", $($body)*);
    };
}

pub mod prelude {
    pub use crate::Result;
    pub use crate::espidf_only;
    pub use crate::target_only;
}

// TODO: pensar numa estrutura de Board support package (BSP) futuramente
// pub mod boards;