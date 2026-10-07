import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))

import check_package_manager_contracts as contracts


class PackageManagerContractTests(unittest.TestCase):
    def test_repository_contracts_are_consistent(self):
        self.assertEqual(contracts.validate(ROOT), [])


if __name__ == "__main__":
    unittest.main()
