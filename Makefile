.PHONY: install run clean help

# Default command to run install dependencies and run program
all: install run

# Installs all packages listed in 'requirements.txt'
install:
	@echo ""
	@echo "Installing all necessary python packages.."
	@echo ""
	@python -m pip install -Ur requirements.txt

# Run the main program
run:
	@echo ""
	@echo "Running main program:"
	@echo ""
	@python main.py

# Clean up files
clean:
	@echo "Cleaning unnecessary files.."
	@find . -name '*.pyc' -exec rm --force {} +
	@find . -name '*.pyo' -exec rm --force {} +
	@name '*~' -exec rm --force  {}

# Help command 
help:
	@echo "Please use 'make <target>' where <target> is one of"
	@echo ""
	@echo "  all         to run all commands altogether"
	@echo "  install     install packages and prepare environment"
	@echo "  run         run main code"
	@echo "  clean       remove *.pyc files and __pycache__ directory"
	@echo ""
	@echo "Check the Makefile to know exactly what each target is doing."


