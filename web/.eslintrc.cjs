/* eslint-env node */

module.exports = {
    root: true,
    parser: "@typescript-eslint/parser",
    extends: [
        "eslint:recommended",
        "plugin:@typescript-eslint/recommended",
        "plugin:react/recommended",
        "plugin:react/jsx-runtime",
        "prettier",
    ],
    ignorePatterns: ["dist/"],
    rules: {
        "@typescript-eslint/ban-ts-comment": [
            "error",
            {
                "ts-expect-error": "allow-with-description",
                "ts-ignore": "allow-with-description",
                "ts-nocheck": "allow-with-description",
                "ts-check": "allow-with-description",
                minimumDescriptionLength: 5,
            },
        ],
        "@typescript-eslint/no-empty-interface": 0,
        "jest/no-deprecated-functions": 0,
    },
    settings: {
        react: {
            version: "detect",
        },
    },
};
