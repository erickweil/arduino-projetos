fn main() {
    #[cfg(feature = "espidf")]
    embuild::espidf::sysenv::output();
}
