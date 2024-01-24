namespace OSK::GRAPHICS {

	struct PbrIblConfig {

		alignas(4) float irradianceStrength = 1.0f;
		alignas(4) float specularStrength = 1.0f;
		alignas(4) float radianceStrength = 1.0f;
		alignas(4) float emissiveStrength = 1.0f;

	};

}
