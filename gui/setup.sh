# Create .venv if does not exist
if [ ! -d ".venv" ]; then
    echo "Creating virtual environment..."
    python3 -m venv .venv
else
    echo "Virtual environment already exists, skipping creation."
fi

# Activate venv
source .venv/bin/activate

# Upgrade pip
python -m pip install --upgrade pip

# Install project dependencies from pyproject.toml
pip install -e .
